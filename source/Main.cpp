#include "plugin.h"
#include "CFileLoader.h"
#include "CMenuManager.h"
#include "CRadar.h"
#include "CWorld.h"

#include "Settings.h"

using namespace plugin;

enum RadarTraceHeight {
	RADAR_TRACE_LOW,
	RADAR_TRACE_HIGH,
	RADAR_TRACE_NORMAL
};

class RadarMark {
	static bool markExist;
	static CVector markPosition;
	static CVector playerPosition;
public:
    RadarMark() {
		static CSprite2d target;
		static RwTexDictionary* txd;

		plugin::Events::initRwEvent += [] {
				Settings::read();

				txd = CFileLoader::LoadTexDictionary(PLUGIN_PATH((char*)"target.txd"));
				target.m_pTexture = GetFirstTexture(txd);
			};

		plugin::Events::drawBlipsEvent += [] {
				CPad* pad = CPad::GetPad(0);
				bool rightMouseButtonClicked = pad->NewMouseControllerState.rmb && !pad->OldMouseControllerState.rmb;
				CVector2D crosshair;
				crosshair.x = FrontEndMenuManager.m_nMouseTempPosX;
				crosshair.y = FrontEndMenuManager.m_nMouseTempPosY;
				playerPosition = FindPlayerCentreOfWorld_NoSniperShift();
				if (rightMouseButtonClicked && FrontEndMenuManager.m_bDrawRadarOrMap) {
					SetMark(crosshair);
				}
				if (markExist) {
					if (Settings::drawMarker) {
						DrawMark(markPosition, Settings::size, Settings::color);
					}
					else {
						DrawSprite(target, markPosition);
					}
					if ((DistanceBetweenPoints(CVector(playerPosition.x, playerPosition.y, 0), CVector(markPosition.x, markPosition.y, 0)) < 10.0) && Settings::removeWhenNearby)
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
		int mode = RADAR_TRACE_LOW;
		if (position.z - playerPosition.z <= 2.0f)
			if (position.z - playerPosition.z < -2.0f) mode = RADAR_TRACE_HIGH;
			else mode = RADAR_TRACE_NORMAL;
		if (size && mode != RADAR_TRACE_NORMAL)
			size--;
		CRadar::ShowRadarTraceWithHeight(screenPosition.x, screenPosition.y, size, color.r, color.g, color.b, color.a, mode);
	}

	static void DrawSprite(CSprite2d& sprite, CVector position) {
		static CVector2D radarPosition, screenPosition;
		CRadar::TransformRealWorldPointToRadarSpace(radarPosition, CVector2D(position.x, position.y));
		CRadar::LimitRadarPoint(radarPosition);
		CRadar::TransformRadarPointToScreenSpace(screenPosition, radarPosition);

		sprite.Draw(CRect(screenPosition.x - 16, screenPosition.y - 16, screenPosition.x + 16, screenPosition.y + 16), CRGBA(255, 255, 255, 255));
	}
} RadarMarkPlugin;

bool RadarMark::markExist = false;
CVector RadarMark::markPosition = CVector(0, 0, 0);
CVector RadarMark::playerPosition = CVector(0, 0, 0);
