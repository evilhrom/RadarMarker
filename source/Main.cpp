#include "plugin.h"
#include "CMenuManager.h"
#include "CRadar.h"
#include "CWorld.h"

using namespace plugin;

enum RadarTraceHeight {
	RADAR_TRACE_LOW,
	RADAR_TRACE_HIGH,
	RADAR_TRACE_NORMAL
};

class RadarMark {
	static bool markExist;
	static CVector markPosition;
public:
    RadarMark() {
		plugin::Events::drawBlipsEvent += [] {
				CPad* pad = CPad::GetPad(0);
				bool rightMouseButtonClicked = pad->NewMouseControllerState.rmb && !pad->OldMouseControllerState.rmb;
				CVector2D crosshair;
				crosshair.x = FrontEndMenuManager.m_nMouseTempPosX;
				crosshair.y = FrontEndMenuManager.m_nMouseTempPosY;
				if (rightMouseButtonClicked && FrontEndMenuManager.m_bDrawRadarOrMap) {
					SetMark(crosshair);
				}
				if (markExist) {
					DrawMark(markPosition, 4, CRGBA(255, 0, 0));
					CVector playerPosition = FindPlayerCentreOfWorld_NoSniperShift();
					if (DistanceBetweenPoints(CVector(playerPosition.x, playerPosition.y, 0), CVector(markPosition.x, markPosition.y, 0)) < 10.0)
						markExist = false;
				}
			};
		
	};

private:
	static void SetMark(CVector2D position)	{
		markExist = !markExist;
		if (markExist) {
			CVector2D RadarPoint, WorldPoint2D;
			RadarPoint.x = (position.x / ((float)RsGlobal.screenWidth * *(float*)0x68FD14) - (*(float*)0x68FD10 * *(float*)0x869670 + *(float*)0x869674)) / (*(float*)0x68FD0C * *(float*)0x869670);
			RadarPoint.y = (*(float*)0x869678 - *(float*)0x68FD1C * *(float*)0x869670 - (position.y / ((float)RsGlobal.screenHeight * *(float*)0x68FD20))) / (*(float*)0x68FD18 * *(float*)0x869670);
			CRadar::TransformRadarPointToRealWorldSpace(WorldPoint2D, RadarPoint);
			markPosition = CVector(WorldPoint2D.x, WorldPoint2D.y, CWorld::FindGroundZForCoord(WorldPoint2D.x, WorldPoint2D.y));
		}
	}

	static void DrawMark(CVector position, uint size, CRGBA color) {
		static CVector2D radarPosition, screenPosition;
		CRadar::TransformRealWorldPointToRadarSpace(radarPosition, CVector2D(position.x, position.y));
		CRadar::LimitRadarPoint(radarPosition);
		CRadar::TransformRadarPointToScreenSpace(screenPosition, radarPosition);
		CVector playerPosition = FindPlayerCentreOfWorld_NoSniperShift();
		int mode = RADAR_TRACE_LOW;
		if (position.z - playerPosition.z <= 2.0f)
			if (position.z - playerPosition.z < -2.0f) mode = RADAR_TRACE_HIGH;
			else mode = RADAR_TRACE_NORMAL;
		if (size && mode != RADAR_TRACE_NORMAL)
			size--;
		CRadar::ShowRadarTraceWithHeight(screenPosition.x, screenPosition.y, size, color.r, color.g, color.b, color.a, mode);
	}
} RadarMarkPlugin;

bool RadarMark::markExist = false;
CVector RadarMark::markPosition = CVector(0, 0, 0);
