#ifndef _scorearea_cpp
#define _scorearea_cpp

#include "scorearea.h"
#include "catchapp.h"

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// ScoreArea Methods

ScoreArea::ScoreArea(World* TheWorld) :
    LevelZone(TheWorld),
    ScoreMultiplier(1.0)
    {  }

ScoreArea::ScoreArea(const EntityID& EntID, World* TheWorld, const Vector3& HalfAreaSize) :
    LevelZone(EntID,TheWorld,HalfAreaSize),
    ScoreMultiplier(1.0)
    { this->CreateScoreArea(HalfAreaSize); }

ScoreArea::ScoreArea(const XML::Node& SelfRoot, World* TheWorld) :
    LevelZone(TheWorld),
    ScoreMultiplier(1.0)
    { this->ProtoDeSerialize(SelfRoot); }

ScoreArea::~ScoreArea()
    { this->DestroyScoreArea(); }

void ScoreArea::CreateScoreArea(const Vector3& HalfAreaSize)
{

}

void ScoreArea::DestroyScoreArea()
{

}

///////////////////////////////////////////////////////////////////////////////
// Utility

void ScoreArea::ApplyEffect()
    { AreaEffect::ApplyEffect(); }

Boole ScoreArea::AllObjectsAtRest()
{
    if( this->OverlappingObjects.empty() )
        return false;

    for( ObjectIterator ObjIt = this->OverlappingObjects.begin() ; ObjIt != this->OverlappingObjects.end() ; ObjIt++ )
    {
        ComponentContainer ColProxies = (*ObjIt)->GetComponents(Mezzanine::CT_Physics_RigidProxy,Mezzanine::CT_Physics_SoftProxy);
        for( ComponentIterator ProxIt = ColProxies.begin() ; ProxIt != ColProxies.end() ; ++ProxIt )
        {
            if( static_cast<Physics::CollidableProxy*>( *ProxIt )->IsActive() )
                return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// LevelZone Properties

void ScoreArea::SetScoreMultiplier(const Real Multiplier)
    { this->ScoreMultiplier = Multiplier; }

Real ScoreArea::GetScoreMultiplier() const
    { return this->ScoreMultiplier; }

///////////////////////////////////////////////////////////////////////////////
// Serialization

void ScoreArea::ProtoSerializeProperties(XML::Node& SelfRoot) const
{
    this->LevelZone::ProtoSerializeProperties(SelfRoot);

    XML::Node PropertiesNode = SelfRoot.AppendChild( ScoreArea::GetSerializableName() + "Properties" );

    if( PropertiesNode.AppendAttribute("Version").SetValue("1") &&
        PropertiesNode.AppendAttribute("ScoreMultiplier").SetValue( this->GetScoreMultiplier() ) )
    {
        return;
    }else{
        SerializeError("Create XML Attribute Values",ScoreArea::GetSerializableName() + "Properties",true);
    }
}

void ScoreArea::ProtoDeSerializeProperties(const XML::Node& SelfRoot)
{
    this->DestroyScoreArea();
    this->LevelZone::ProtoDeSerializeProperties(SelfRoot);

    XML::Attribute CurrAttrib;
    XML::Node PropertiesNode = SelfRoot.GetChild( ScoreArea::GetSerializableName() + "Properties" );

    if( !PropertiesNode.Empty() ) {
        if(PropertiesNode.GetAttribute("Version").AsInt() == 1) {
            CurrAttrib = PropertiesNode.GetAttribute("ScoreMultiplier");
            if( !CurrAttrib.Empty() )
                this->SetScoreMultiplier( CurrAttrib.AsReal() );
        }else{
            MEZZ_EXCEPTION(ExceptionBase::INVALID_VERSION_EXCEPTION,"Incompatible XML Version for " + (ScoreArea::GetSerializableName() + "Properties" ) + ": Not Version 1.");
        }
    }else{
        MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,ScoreArea::GetSerializableName() + "Properties" + " was not found in the provided XML node, which was expected.");
    }
}

String ScoreArea::GetDerivedSerializableName() const
    { return ScoreArea::GetSerializableName(); }

String ScoreArea::GetSerializableName()
    { return "ScoreArea"; }

///////////////////////////////////////////////////////////////////////////////
// ScoreAreaFactory Methods

ScoreAreaFactory::ScoreAreaFactory()
    {  }

ScoreAreaFactory::~ScoreAreaFactory()
    {  }

String ScoreAreaFactory::GetTypeName() const
    { return ScoreArea::GetSerializableName(); }

ScoreArea* ScoreAreaFactory::CreateScoreArea(const EntityID& EntID, World* TheWorld, const Vector3& HalfAreaSize)
    { return new ScoreArea(EntID,TheWorld,HalfAreaSize); }

ScoreArea* ScoreAreaFactory::CreateScoreArea(const XML::Node& XMLNode, World* TheWorld)
    { return static_cast<ScoreArea*>( this->CreateEntity(XMLNode,TheWorld) ); }

Entity* ScoreAreaFactory::CreateEntity(const EntityID& EntID, World* TheWorld, const NameValuePairMap& Params)
{
    Vector3 HalfSize;
    NameValuePairMap::const_iterator ParamIt;
    ParamIt = Params.find( "SizeX" );
    if( ParamIt != Params.end() )
        HalfSize.X = StringTools::ConvertToReal( (*ParamIt).second );

    ParamIt = Params.find( "SizeY" );
    if( ParamIt != Params.end() )
        HalfSize.Y = StringTools::ConvertToReal( (*ParamIt).second );

    ParamIt = Params.find( "SizeZ" );
    if( ParamIt != Params.end() )
        HalfSize.Z = StringTools::ConvertToReal( (*ParamIt).second );

    return new ScoreArea(EntID,TheWorld,HalfSize);
}

Entity* ScoreAreaFactory::CreateEntity(const XML::Node& XMLNode, World* TheWorld)
    { return new ScoreArea(XMLNode,TheWorld); }

void ScoreAreaFactory::DestroyEntity(Entity* ToBeDestroyed)
    { delete ToBeDestroyed; }

#endif
