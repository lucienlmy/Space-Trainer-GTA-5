#include "SpoonerEntity.h"

#include "..\..\macros.h"

//#include "..\..\Menu\Menu.h"
//#include "..\..\Menu\Routine.h"

//#include "..\..\Natives\natives2.h"

namespace sub::Spooner
{
	std::string SpoonerEntity::TypeName()
	{
		/*auto type = static_cast<UINT8>(this->Type);
		const std::array<std::string, 4> vTypeNames{"UNK", "PED", "VEHICLE", "OBJECT"};
		if (type < 1 || type > 3) return vTypeNames.front();
		return vTypeNames[type];*/

		switch (this->type)
		{
		case EntityType::PED: return "PED"; break;
		case EntityType::VEHICLE: return "VEHICLE"; break;
		case EntityType::PROP: return "OBJECT"; break;
		default: return "UNK"; break;
		}
	}

	SpoonerEntity::SpoonerEntity()
	{
		this->type = (EntityType)0;
		this->dynamic = false;
		//this->Door = false;
		this->attachmentArgs.isAttached = false;
		this->attachmentArgs.boneIndex = 0;
		this->attachmentArgs.offset = Vector3();
		this->attachmentArgs.rotation = Vector3();
		this->textureVariation = 0;
		this->isStill = false;
	}
	//const SpoonerEntity& SpoonerEntity::operator = (const SpoonerEntity& right)
	SpoonerEntity::SpoonerEntity(const SpoonerEntity& right)
	{
		this->handle = right.handle;
		this->type = (EntityType)right.type;
		this->hashName = right.hashName;
		this->dynamic = right.dynamic;
		this->lastAnimations = right.lastAnimations;
		this->currentScenario = right.currentScenario;
		this->attachmentArgs.isAttached = right.attachmentArgs.isAttached;
		this->attachmentArgs.boneIndex = right.attachmentArgs.boneIndex;
		this->attachmentArgs.offset = right.attachmentArgs.offset;
		this->attachmentArgs.rotation = right.attachmentArgs.rotation;
		this->textureVariation = right.textureVariation;
		this->isStill = right.isStill;
		this->taskSequence = right.taskSequence;

		//return *this;
	}

	// helper for automatically removing old animation with the same flag before adding a new one
	void SpoonerEntity::AddOrUpdateLastAnimation(const Animation& anim)
	{
		RemoveFromLastAnimations(anim.flag);
		lastAnimations.push_back(anim);
	}

	void SpoonerEntity::RemoveFromLastAnimations(int flag)
	{
		for (auto it = lastAnimations.begin(); it != lastAnimations.end(); ++it)
		{
			if (it->flag == flag)
			{
				lastAnimations.erase(it);
				return;
			}
		}
	}

	bool SpoonerEntity::HasInLastAnimations(int flag) const
	{
		for (const auto& a : lastAnimations)
		{
			if (a.flag == flag) return true;
		}
		return false;
	}

	void SpoonerEntity::ClearLastAnimations()
	{
		lastAnimations.clear();
	}

	bool operator == (const SpoonerEntity& left, const SpoonerEntity& right)
	{
		return left.handle == right.handle;
	}
	bool operator != (const SpoonerEntity& left, const SpoonerEntity& right)
	{
		return left.handle != right.handle;
	}
	bool operator == (const SpoonerEntity& left, const GTAentity& right)
	{
		return left.handle == right;
	}
	bool operator != (const SpoonerEntity& left, const GTAentity& right)
	{
		return left.handle != right;
	}


	SpoonerEntity selectedEntity;
	std::vector<SpoonerEntity> selectedSpoonGroup;

}



