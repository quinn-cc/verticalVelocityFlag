/*
 Custom flag: Vertical Velocity (+VV)
 Extra two shots travel with vertical velocity.
 
 Server Variables:
 _verticalVelocityWidth - the distance from the middle shot and a side VV shot
 
 Extra notes:
 - The VV shots have metadata "owner" and "type". Owner is the playerID that issued
   the shot, and type is VV.
 
 Copyright 2022 Quinn Carmack
 May be redistributed under either the LGPL or MIT licenses.
 
 ./configure --enable-custom-plugins=verticalVelocity
*/

#include "bzfsAPI.h"
#include <math.h>

using namespace std;

class VerticalVelocity : public bz_Plugin
{
	virtual const char* Name()
	{
		return "Vertical Velocity Flag";
	}

	virtual void Init(const char*);
	virtual void Event(bz_EventData*);
	~VerticalVelocity();

	virtual void Cleanup(void)
	{
		Flush();
	}
};

BZ_PLUGIN(VerticalVelocity)

void VerticalVelocity::Init(const char*)
{
	bz_RegisterCustomFlag("VV", "Vertical Velocity", "Extra two shots travel with vertical velocity.", 0, eGoodFlag);
	bz_registerCustomBZDBDouble("_verticalVelocityWidth", 2.0);
	Register(bz_eShotFiredEvent);
	Register(bz_ePlayerDieEvent);
}

VerticalVelocity::~VerticalVelocity() {}

void VerticalVelocity::Event(bz_EventData *eventData)
{
	switch (eventData->eventType)
	{
		case bz_eShotFiredEvent:
		{
			bz_ShotFiredEventData_V1* data = (bz_ShotFiredEventData_V1*) eventData;
			bz_BasePlayerRecord *playerRecord = bz_getPlayerByIndex(data->playerID);

			if (playerRecord && playerRecord->currentFlag == "Vertical Velocity (+VV)")
			{
				float velocity[3];
				float pos[3];      // VV shot's base position
				float offset[2];   // VV shot's offset from base position
				float pos1[3];     // Position of one VV shot
				float pos2[3];     // Position of the other VV shot
				
				velocity[0] = cos(playerRecord->lastKnownState.rotation)+playerRecord->lastKnownState.velocity[0]/bz_getBZDBDouble("_shotSpeed");
				velocity[1] = sin(playerRecord->lastKnownState.rotation)+playerRecord->lastKnownState.velocity[1]/bz_getBZDBDouble("_shotSpeed");
				velocity[2] = playerRecord->lastKnownState.velocity[2]/bz_getBZDBDouble("_shotSpeed");
				
				pos[0] = playerRecord->lastKnownState.pos[0] + cos(playerRecord->lastKnownState.rotation)*bz_getBZDBDouble("_muzzleFront");
				pos[1] = playerRecord->lastKnownState.pos[1] + sin(playerRecord->lastKnownState.rotation)*bz_getBZDBDouble("_muzzleFront");
				pos[2] = playerRecord->lastKnownState.pos[2] + bz_getBZDBDouble("_muzzleHeight");
				
				offset[0] = -sin(playerRecord->lastKnownState.rotation)*bz_getBZDBDouble("_verticalVelocityWidth");
				offset[1] = cos(playerRecord->lastKnownState.rotation)*bz_getBZDBDouble("_verticalVelocityWidth");
				
				// Shot 1
				pos1[0] = pos[0] + offset[0];
				pos1[1] = pos[1] + offset[1];
				pos1[2] = pos[2];
				uint32_t shot1GUID = bz_fireServerShot("VV", pos1, velocity, playerRecord->team, data->playerID);
			  	bz_setShotMetaData(shot1GUID, "type", bz_getPlayerFlag(data->playerID));
				bz_setShotMetaData(shot1GUID, "owner", data->playerID);
				
				// Shot 2
				pos2[0] = pos[0] - offset[0];
				pos2[1] = pos[1] - offset[1];
				pos2[2] = pos[2];
				uint32_t shot2GUID = bz_fireServerShot("VV", pos2, velocity, playerRecord->team, data->playerID);
				bz_setShotMetaData(shot2GUID, "type", bz_getPlayerFlag(data->playerID));
				bz_setShotMetaData(shot2GUID, "owner", data->playerID);
			}
			
			bz_freePlayerRecord(playerRecord);
		} break;
	  	case bz_ePlayerDieEvent:
		{
			bz_PlayerDieEventData_V1* data = (bz_PlayerDieEventData_V1*) eventData;
			uint32_t shotGUID = bz_getShotGUID(data->killerID, data->shotID);

			if (bz_shotHasMetaData(shotGUID, "type") && bz_shotHasMetaData(shotGUID, "owner"))
			{
			    std::string flagType = bz_getShotMetaDataS(shotGUID, "type");

			    if (flagType == "VV")
			    {
			        data->killerID = bz_getShotMetaDataI(shotGUID, "owner");
			        data->killerTeam = bz_getPlayerTeam(data->killerID);
			    }
			}
		} break;
		default:
			break;
    }
}
