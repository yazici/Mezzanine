#ifndef _scorearea_h
#define _scorearea_h

#include "levelzone.h"

///////////////////////////////////////////////////////////////////////////////
/// @brief This is a level zone representing the end zone where throwables contribute to the level score.
///////////////////////////////////////
class ScoreArea : public LevelZone
{
protected:
    /// @brief The score modifier applied to the score values of each throwable in this zone.
    Real ScoreMultiplier;

    /// @brief Common constructor method for ScoreArea base class.
    /// @param HalfAreaSize Half of the size on each axis of the score area.
    virtual void CreateScoreArea(const Vector3& HalfAreaSize);
    /// @brief Common destructor method for ScoreArea base class.
    virtual void DestroyScoreArea();
public:
    /// @brief Blank constructor.
    /// @param TheWorld A pointer to the world this object belongs to.
    ScoreArea(World* TheWorld);
    /// @brief Class constructor.
    /// @param EntID The unique ID of the ScoreArea.
    /// @param TheWorld A pointer to the world this object belongs to.
    /// @param HalfAreaSize Half of the size on each axis of the score area.
    ScoreArea(const EntityID& EntID, World* TheWorld, const Vector3& HalfAreaSize);
    /// @brief XML constructor.
    /// @param SelfRoot An XML::Node containing the data to populate this class with.
    /// @param TheWorld A pointer to the world this object belongs to.
    ScoreArea(const XML::Node& SelfRoot, World* TheWorld);
    /// @brief Class destructor.
    virtual ~ScoreArea();

    ///////////////////////////////////////////////////////////////////////////////
    // Utility

    /// @copydoc Mezzanine::AreaEffect::ApplyEffect()
    virtual void ApplyEffect();
    /// @brief Gets whether or not all objects in this zone are at rest.
    /// @return Returns true if all objects in this score zone are at rest, false otherwise.
    virtual Boole AllObjectsAtRest();

    ///////////////////////////////////////////////////////////////////////////////
    // LevelZone Properties

    /// @brief Sets the score modifier for throwables in this zone.
    /// @param Multiplier The score modifier that will be applied to each throwable in this zone.
    virtual void SetScoreMultiplier(const Real Multiplier);
    /// @brief Gets the score modifier for throwables in this zone.
    /// @return Returns a Real representing the score modifier for all objects in this zone.
    virtual Real GetScoreMultiplier() const;

    ///////////////////////////////////////////////////////////////////////////////
    // Serialization

    /// @copydoc Mezzanine::WorldEntity::ProtoSerializeProperties(XML::Node&) const
    virtual void ProtoSerializeProperties(XML::Node& SelfRoot) const;
    /// @copydoc Mezzanine::WorldEntity::ProtoDeSerializeProperties(const XML::Node&)
    virtual void ProtoDeSerializeProperties(const XML::Node& SelfRoot);

    /// @copydoc Mezzanine::WorldEntity::GetDerivedSerializableName() const
    virtual String GetDerivedSerializableName() const;
    /// @copydoc Mezzanine::WorldEntity::GetSerializableName()
    static String GetSerializableName();
};//ScoreArea

///////////////////////////////////////////////////////////////////////////////
/// @brief A factory type for the creation of ScoreArea objects.
///////////////////////////////////////
class ScoreAreaFactory : public AreaEffectFactory
{
public:
    /// @brief Class constructor.
    ScoreAreaFactory();
    /// @brief Class destructor.
    virtual ~ScoreAreaFactory();

    /// @copydoc Mezzanine::EntityFactory::GetTypeName() const
    virtual String GetTypeName() const;

    /// @brief Creates a ScoreArea.
    /// @param EntID The unique ID of the ScoreArea.
    /// @param TheWorld A pointer to the world this ScoreArea belongs to.
    /// @param HalfAreaSize Half of the size on each axis of the ScoreArea.
    virtual ScoreArea* CreateScoreArea(const EntityID& EntID, World* TheWorld, const Vector3& HalfAreaSize);
    /// @brief Creates a ScoreArea.
    /// @param XMLNode The node of the xml document to construct from.
    /// @param TheWorld A pointer to the world this ScoreArea belongs to.
    virtual ScoreArea* CreateScoreArea(const XML::Node& XMLNode, World* TheWorld);

    /// @copydoc Mezzanine::EntityFactory::CreateEntity(const EntityID&, World*, const NameValuePairMap&)
    virtual Entity* CreateEntity(const EntityID& EntID, World* TheWorld, const NameValuePairMap& Params) override;
    /// @copydoc Mezzanine::EntityFactory::CreateEntity(const XML::Node&)
    virtual Entity* CreateEntity(const XML::Node& XMLNode, World* TheWorld) override;
    /// @copydoc Mezzanine::EntityFactory::DestroyAreaEffect(Entity*)
    virtual void DestroyEntity(Entity* ToBeDestroyed) override;
};//ScoreAreaFactory

#endif
