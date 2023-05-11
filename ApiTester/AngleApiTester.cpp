/*****************************************
  Example Program : Angle Aligner API
  DLL file : AngleApi_x64.dll
*****************************************/

#pragma region ---- Header ----

#include "Common.h"

enum class AX
{
    None = 0,
    t = 1,
    X, Y, Z,
    R, P, W //Rx Ry Rz
};

typedef double(__cdecl* ApiAx)(AX axis);

#pragma endregion


#pragma region ---- API Functions ----

//init/open/close
static HMODULE _api;
static Api0 InitializeApi;
static Api0 OpenDevice;
static Api0 Close;

//driving
static Api0 ResetAlarm;
static Api0 Stop;
static Api0 CancelAndStop;

static Api MoveToHome;
static Api TranslateTo;
static Api TranslateAs;
static Api RotateTo;
static Api RotateAs;

//status
static Api0 IsMoving;
static ApiAx GetCoord;
static ApiAx GetVelocity;

#pragma endregion

/// <summary>
/// Load library & get all api functions
/// </summary>
static void load_AngleApi()
{
    int exitCode = -1000;

    _api = LoadLibrary(L"AngleApi_x64.dll");
    if (_api == 0) printAndExit("Failed to load library AngleApi_x64.dll", exitCode--);

    auto load = [&exitCode](const char* funcName)
    {
        auto f = GetProcAddress(_api, funcName);
        if (f == 0) printAndExit(funcName, exitCode--);
        return f;
    };

    InitializeApi = (Api0)load("InitializeApi");
    OpenDevice = (Api0)load("OpenDevice");
    Close = (Api0)load("Close");

    ResetAlarm = (Api0)load("ResetAlarm");
    Stop = (Api0)load("Stop");
    CancelAndStop = (Api0)load("CancelAndStop");

    MoveToHome = (Api)load("MoveToHome");
    TranslateTo = (Api)load("TranslateTo");
    TranslateAs = (Api)load("TranslateAs");
    RotateTo = (Api)load("RotateTo");
    RotateAs = (Api)load("RotateAs");

    IsMoving = (Api0)load("IsMoving");
    GetCoord = (ApiAx)load("GetCoord");
    GetVelocity = (ApiAx)load("GetVelocity");
}


int main_AngleApp()
{
    print("==== Starting Angle API Tester ====");

    try
    {
        //step 1. Load API
        print("---- Loading API ----");
        load_AngleApi();

        //step 2. Initialize API
        print("---- Initializing API ----");
        if (!InitializeApi()) printAndExit("InitializeApi");

        //step 3. Open device
        print("\n---- Opening Devices ----");
        if (!OpenDevice()) printAndExit("OpenDevice");

        //step 4. do something
        print("\n---- Moving To Home ----");
        if (!MoveToHome(0)) printAndExit("MoveToHome");//@(0 0 0 0 0 0)

        //step 5. do something

        //변위 이동: 병진
        print("\n---- TranslateAs Displacements ----");
        if (!TranslateAs("X Y Z, -5 -10 -15")) printAndExit("TranslateAs");//@(-5 -10 -15 0 0 0)

        //좌표 이동: 병진
        print("\n---- TranslateTo Coordinates ----");
        if (!TranslateTo("X Y Z, 5 10 15")) printAndExit("TranslateTo");//@(5 10 15 0 0 0)

        //변위 이동: 회전
        print("\n---- RotateAs Displacements ----");
        MoveToHome(0);//@(0 0 0 0 0 0)
        if (!RotateAs("W, -5")) printAndExit("RotateAs");//@(0 0 0 0 0 -5)

        //좌표 이동: 회전
        print("\n---- RotateTo Coordinates ----");
        if (!RotateTo("W, 5")) printAndExit("RotateTo");//@(0 0 0 0 0 5)

#pragma region ---- async operation ----

        //start async operation
        print("\n---- Starting Asynchronuous Operation ----");
        auto f = [] { if (!RotateTo("W, -5")) print("  --> exception in std::async(RotateTo)"); };
        auto a = std::async(f);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        //좌표 읽기
        auto refresh = [](int dt = 100, bool lf = false)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(dt));
            std::cout
                << "\r[W] Coord=" << format(GetCoord(AX::W))
                << " Velocity=" << format(GetVelocity(AX::W));
            if (lf) printf("\n");
        };

        //wait 5 seconds
        //printf("\n");
        for (size_t i = 1; i <= 50; i++) refresh();
        printf("\n");

        //cancel all operation
        if (!CancelAndStop()) printAndExit("CancelAndStop");
        refresh(100, true);

#pragma endregion

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

