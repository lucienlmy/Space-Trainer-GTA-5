#pragma once

#include "..\..\Util\GTAmath.h"
#include "..\..\Scripting\GTAentity.h"
#include "..\..\Natives\types.h" //RGBA

#include <string>

typedef unsigned __int8 UINT8;
typedef unsigned int UINT;

namespace sub::Spooner
{
	class SpoonerMarkerPosition
	{
	public:
		struct Attachment_t
		{
			GTAentity attachedTo;
			Vector3 offset;
			Vector3 rotation;

			Attachment_t()
				: attachedTo(0)//,offset(Vector3()), rotation(Vector3())
			{
			}

			Attachment_t& operator = (const Attachment_t& right)
			{
				this->attachedTo = right.attachedTo;
				this->offset = right.offset;
				this->rotation = right.rotation;
				return *this;
			}
		};

		Vector3 m_position;
		Vector3 m_rotation;
		Attachment_t m_attachmentArgs;

		SpoonerMarkerPosition()
		{
		}

		SpoonerMarkerPosition& operator = (const SpoonerMarkerPosition& right)
		{
			this->m_position = right.m_position;
			this->m_rotation = right.m_rotation;
			this->m_attachmentArgs = right.m_attachmentArgs;
			return *this;
		}
	};
	class SpoonerMarker : public SpoonerMarkerPosition
	{
	public:
		static bool bPlayerJustTeleportedBetweenMarkers;
		static UINT iMarkerIdIter;

		UINT m_id;
		std::string m_name;
		UINT8 m_type;
		float m_scale;
		bool m_showName;
		bool m_rotateContinuously;
		bool m_allowVehicles;
		bool m_selectedInSub;
		RGBA m_colour;

		SpoonerMarkerPosition m_destinationVal;
		SpoonerMarker* m_destinationPtr;
		float m_destinationHeading;

		SpoonerMarker();
		SpoonerMarker(const std::string& name, const Vector3& pos, const Vector3& rot);

		friend bool operator == (const SpoonerMarker& left, const SpoonerMarker& right);
		SpoonerMarker& operator = (const SpoonerMarker& right);
	};

	class SpoonerMarkerWithInitHandle
	{
	public:
		SpoonerMarker m;
		UINT initHandle;
		UINT linkToHandle;
	};


	extern SpoonerMarker* SelectedMarker;

}



