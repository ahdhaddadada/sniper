#pragma once
#include "plugin.h"
#include "CPlayerPed.h"
#include "CCamera.h"
#include "CPad.h"
#include "CWorld.h"

using namespace plugin;

class SniperTriggerBot {
public:
    static void Init() {
        Events::gameProcessEvent += OnGameProcess;
    }

private:
    static DWORD lastShotTime;

    static void OnGameProcess() {
        if (!(GetAsyncKeyState(VK_LSHIFT) & 0x8000)) return;

        CPad* pad = CPad::GetPad(0);
        if (!pad) return;
        if (!pad->NewMouseControllerState.rmb) return;

        CPlayerPed* player = FindPlayerPed();
        if (!player) return;

        int weaponId = player->GetWeapon().m_nType;
        if (weaponId != 34) return;

        DWORD now = GetTickCount();
        if (now - lastShotTime < 100) return;

        CVector camPos = TheCamera.GetPosition();
        CVector aimPos;
        GetSniperAimPoint(aimPos);

        CColPoint colPoint;
        CEntity* hitEntity = nullptr;
        bool result = World::ProcessLineOfSight(
            camPos, aimPos, colPoint, hitEntity,
            false, false, true, false, false, false, false
        );
        if (!result) return;

        if (colPoint.m_nEntityType != 3) return;
        CPed* hitPed = reinterpret_cast<CPed*>(hitEntity);
        if (!hitPed || hitPed == player) return;
        if (hitPed->m_fHealth <= 0.0f) return;

        lastShotTime = now;
        pad->NewMouseControllerState.lmb = 1;
        pad->NewMouseControllerState.lmbLast = 0;
    }

    static void GetSniperAimPoint(CVector& out) {
        float sw = (float)RsGlobal.maximumWidth;
        float sh = (float)RsGlobal.maximumHeight;
        float crossX = 339.1f * (sw / 640.0f);
        float crossY = 179.1f * (sh / 480.0f);
        ScreenToWorld(crossX, crossY, 100.0f, out);
    }

    static void ScreenToWorld(float sx, float sy, float depth, CVector& out) {
        RwCamera* cam = TheCamera.m_pRwCamera;
        if (!cam) return;

        float sw = (float)RsGlobal.maximumWidth;
        float sh = (float)RsGlobal.maximumHeight;
        float aspect = sw / sh;
        float tanHalfFov = tanf(TheCamera.m_fFOV * 0.5f);

        float nx = (sx / sw) * 2.0f - 1.0f;
        float ny = 1.0f - (sy / sh) * 2.0f;

        CVector dir = { nx * tanHalfFov * aspect, ny * tanHalfFov, 1.0f };

        RwMatrix* m = RwFrameGetMatrix(RwCameraGetFrame(cam));
        if (!m) return;

        CVector worldDir;
        worldDir.x = m->right.x * dir.x + m->up.x * dir.y + m->at.x * dir.z;
        worldDir.y = m->right.y * dir.x + m->up.y * dir.y + m->at.y * dir.z;
        worldDir.z = m->right.z * dir.x + m->up.z * dir.y + m->at.z * dir.z;

        out = TheCamera.GetPosition() + worldDir * depth;
    }
};

DWORD SniperTriggerBot::lastShotTime = 0;
