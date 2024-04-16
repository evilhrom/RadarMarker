#include "plugin.h"
#include "CMenuManager.h"
#include "CRadar.h"
#include "CWorld.h"

using namespace plugin;

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

		CRadar::ShowRadarTraceWithHeight(screenPosition.x, screenPosition.y, size, color.r, color.g, color.b, color.a, 2);
	}
} RadarMarkPlugin;

bool RadarMark::markExist = false;
CVector RadarMark::markPosition = CVector(0, 0, 0);
