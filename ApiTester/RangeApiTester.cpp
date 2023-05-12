/*****************************************
  Example Program : Range Aligner API
  DLL file : RangeApi_x64.dll
*****************************************/

#pragma region ---- Header ----

#include "Common.h"

enum class RX
{
    None = 0,
    τ = 1,
    T,  //TARGET
    P,  //PITCH Rx
    W   //YAW   Ry
};

typedef double(__cdecl* ApiRx)(RX axis);

#pragma endregion


#pragma region ---- API Functions ----

//---- API Init/Open/Close ----
static HMODULE _api;
static Api0 InitializeApi;
static Api0 OpenDevice;
static Api0 Close;

//---- Driving ----
static Api Stop;
static Api0 CancelAndStop;

static Api FindHome;
static Api MoveToHome;
static Api ZeroSet;

static Api MoveAs;
static Api MoveToAbs;
static Api MoveToRel;

//---- State Control ----
static Api ResetAlarm;
static Api BrakeOn;
static Api BrakeOff;
static Api ServoOn;
static Api ServoOff;

//---- Status Query ----
static ApiRx IsMoving;
static ApiRx GetAbsCoord;
static ApiRx GetRelCoord;
static ApiRx GetVelocity;

#pragma endregion

/// <summary>
/// Load library & get all api functions
/// </summary>
static void load_RangeApi()
{
    _api = LoadLibrary(L"RangeApi_x64.dll");
    if (_api == 0) printAndExit("LoadLibrary(RangeApi_x64.dll)");

    auto load = [](const char* funcName)
    {
        auto f = GetProcAddress(_api, funcName);
        if (f == 0) printAndExit(funcName);
        return f;
    };

    //---- API Init/Open/Close ----
    InitializeApi = (Api0)load("InitializeApi");
    OpenDevice = (Api0)load("OpenDevice");
    Close = (Api0)load("Close");

    //---- Driving ----
    Stop = (Api)load("Stop");
    CancelAndStop = (Api0)load("CancelAndStop");
    FindHome = (Api)load("FindHome");
    MoveToHome = (Api)load("MoveToHome");
    MoveAs = (Api)load("MoveAs");
    MoveToAbs = (Api)load("MoveToAbs");
    MoveToRel = (Api)load("MoveToRel");

    //---- State Control ----
    ZeroSet = (Api)load("ZeroSet");
    BrakeOn = (Api)load("BrakeOn");
    BrakeOff = (Api)load("BrakeOff");
    ServoOn = (Api)load("ServoOn");
    ServoOff = (Api)load("ServoOff");
    ResetAlarm = (Api)load("ResetAlarm");

    //---- Status Query ----
    IsMoving = (ApiRx)load("IsMoving");
    GetAbsCoord = (ApiRx)load("GetAbsCoord");
    GetRelCoord = (ApiRx)load("GetRelCoord");
    GetVelocity = (ApiRx)load("GetVelocity");
}


int main_RangeApp()
{
    print("==== Starting Range API Tester ====");

    try
    {
        //step 1. Load API
        print("---- Loading API ----");
        load_RangeApi();

        //step 2. Initialize API
        print("---- Initializing API ----");
        if (!InitializeApi()) printAndExit("InitializeApi", 2);

        //step 3. Open device
        print("\n---- Opening Devices ----");
        if (!OpenDevice()) printAndExit("OpenDevice", 3);

        //step 4. find Home
        print("\n---- Finding Home ----");
        if (!FindHome("")) printAndExit("FindHome", 4);//@(0 0 400)

        //step 5. do something

        //변위 이동
        print("\n---- MoveAs Displacements ----");
        if (!MoveAs("T, 1000")) printAndExit("MoveAs");//@(0 0 1400)
        if (!MoveAs("W P, 5 5")) printAndExit("MoveAs"); //[W P]축 [5, 5]º 만큼 이동 @(5 5 1400)

        //절대좌표 이동
        print("\n---- MoveToAbs Coordinates ----");
        if (!MoveToAbs("W P T, -5 -5 1000")) printAndExit("MoveToAbs"); //Δ= -10 -10 -400 @(-5 -5 1000)

        //좌표 읽기
        auto refresh = [](int dt = 100, bool lf = false)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(dt));
            std::cout
                << "\r[T] Abs=" << format(GetAbsCoord(RX::T))
                << " Rel=" << format(GetRelCoord(RX::T))
                << " Vel=" << format(GetVelocity(RX::T));
            if (lf) printf("\n");
        };

        //상대좌표 이동
        if (!ZeroSet("W P T")) printAndExit("ZeroSet");	//[W P T]축 현재위치를 상대좌표[0 0 0]으로 설정 @(-5 -5 1000)
        print("\n---- MoveToRel Coordinates ----");
        if (!MoveToRel("W P T, 20 10 1000")) printAndExit("MoveToRel"); //[W P T] 상대좌표 [20º 10º 1000mm] 위치로 동시 이동 @(15 5 2000)

#pragma region ---- async operation ----

        //start async operation
        print("\n---- Starting Asynchronuous Operation ----");
        if (!MoveToHome("")) printAndExit("MoveToHome");//@(0 0 400)
        auto f = [] { if (!MoveAs("T, 1000")) print("  --> exception in std::async(MoveAs)"); };
        auto a = std::async(f);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));		

        //wait 5 seconds
        for (size_t i = 1; i <= 50; i++) refresh();
        printf("\n");

        //cancel all operation
        if (!CancelAndStop()) printAndExit("CancelAndStop", 5);
        refresh(100, true);

#pragma endregion

        // ...

        //step 6. Finalize API
        print("\n---- Closing API ----");
        Close();
    }
    catch (const std::exception& ex)
    {
        printf(ex.what());
        return -1;
    }

    return 0;
}
