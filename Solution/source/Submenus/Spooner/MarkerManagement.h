#pragma once

#include <string>
#include <vector>

class Vector3;

namespace sub::Spooner
{
	class SpoonerMarker;

	namespace MarkerManagement
	{
		void DrawAll();

		SpoonerMarker* AddMarker(const std::string& name, const Vector3& position, const Vector3& rotation);
		SpoonerMarker* AddMarker(const Vector3& position, const Vector3& rotation);

		void GetAllMarkersInRange(std::vector<SpoonerMarker>& result, const Vector3& position, float radius);

		void ClearDb();
		inline std::vector<SpoonerMarker>::iterator RemoveMarker(std::vector<SpoonerMarker>::iterator it);
		void RemoveMarker(SpoonerMarker& marker);
		void RemoveMarker(int indexInDb);
		void RemoveAllMarkers();
		void RemoveAllMarkersInRange(const Vector3& position, float radius);

		SpoonerMarker* CopyMarker(SpoonerMarker& marker);
	}

}



