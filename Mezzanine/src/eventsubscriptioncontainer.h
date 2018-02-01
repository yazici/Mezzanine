// � Copyright 2010 - 2017 BlackTopp Studios Inc.
/* This file is part of The Mezzanine Engine.

    The Mezzanine Engine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Mezzanine Engine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The Mezzanine Engine.  If not, see <http://www.gnu.org/licenses/>.
*/
/* The original authors have included a copy of the license specified above in the
   'Docs' folder. See 'gpl.txt'
*/
/* We welcome the use of the Mezzanine engine to anyone, including companies who wish to
   Build professional software and charge for their product.

   However there are some practical restrictions, so if your project involves
   any of the following you should contact us and we will try to work something
   out:
    - DRM or Copy Protection of any kind(except Copyrights)
    - Software Patents You Do Not Wish to Freely License
    - Any Kind of Linking to Non-GPL licensed Works
    - Are Currently In Violation of Another Copyright Holder's GPL License
    - If You want to change our code and not add a few hundred MB of stuff to
        your distribution

   These and other limitations could cause serious legal problems if you ignore
   them, so it is best to simply contact us or the Free Software Foundation, if
   you have any questions.

   Joseph Toppi - toppij@gmail.com
   John Blackwood - makoenergy02@gmail.com
*/
#ifndef _eventsubscriptioncontainer_h
#define _eventsubscriptioncontainer_h

#include "exception.h"
#include "sortedvector.h"
#include "managedarray.h"
#include "sortedmanagedarray.h"
#include "eventsubscriptionfactory.h"

namespace Mezzanine
{
    /// @addtogroup Events
    /// @{

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief This is an empty container that is the basis from which containers are specialized.
    ///////////////////////////////////////
    template<class TableType, class Traits, SubscriptionContainerType ContainerType>
    class EventSubscriptionContainer
        {  };

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief This is a container class for the storage of multiple sorted subscribers.
    ///////////////////////////////////////
    template<class TableType, class Traits>
    class EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Single>
    {
    public:
        /// @brief Convenience type for the EventSubscriptionContainer implementation being used.
        using SelfType = EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Single>;
        /// @brief Retrievable type for querying the type of callable interface that is stored by this container.
        using SubscriberType = typename Traits::SubscriberType;
        /// @brief Guaranteed to be a non-pointer, non-reference SubscriberType.
        using SubscriberValue = typename Traits::SubscriberValue;
        /// @brief A pointer to the underlying Subscriber type.
        using SubscriberPtr = typename Traits::SubscriberPtr;
        /// @brief The type to use for uniquely identifying instances of subscribers.
        using SubscriberIDType = typename Traits::SubscriberIDType;
        /// @brief Convenience type and check for what exactly will be stored by this container.
        using StoredType = typename Traits::StoredType;
        /// @brief Convenience type for passing the subscriber as an argument to the Subscribe method.
        using SubscribeArg = typename Traits::SubscribeArg;
        /// @brief Convenience type for the return value of the Subscribe method.
        using SubscribeRet = typename Traits::SubscribeRet;
        /// @brief Convenience type for getting a subscription stored in a table.
        using SubscriptionGet = typename Traits::SubscriptionGet;

        /// @brief Check to make sure we have a pointer subscriber type, as this is written to expect assigning nullptr to the subscriber will work.
        static_assert(std::is_pointer<StoredType>::value,"The Single subscriber template specialization only works with pointer subscribers and no bindings.");
    protected:
        /// @brief The storage for the one and only subscription this container will track.
        StoredType Subscription = nullptr;
    public:
        /// @brief Blank constructor.
        EventSubscriptionContainer() = default;
        /// @brief Copy constructor.
        /// @param Other The other table to NOT be copied.
        EventSubscriptionContainer(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move constructor.
        /// @param Other The other table to be moved.
        EventSubscriptionContainer(EventSubscriptionContainer&& Other) = default;
        /// @brief Class destructor.
        virtual ~EventSubscriptionContainer() = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Operators

        /// @brief Assignment operator.
        /// @param Other The other table to NOT be copied.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move assignment operator.
        /// @param Other The other table to be moved.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(EventSubscriptionContainer&& Other) = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Subscription Management

        /// @brief Adds a subscriber to this container.
        /// @exception If the ID of the Sub is already being used by another subscriber this will throw a "II_DUPLICATE_IDENTITY_EXCEPTION".
        /// @param Sub The subscriber to be called when the interested event is fired.
        /// @return Returns an instance of the subscription storage.
        SubscribeRet Subscribe(SubscribeArg Sub)
        {
            if( this->Subscription->GetID() == EventHelper::ToPointer(Sub)->GetID() ) {
                MEZZ_EXCEPTION(ExceptionBase::II_DUPLICATE_IDENTITY_EXCEPTION,"A subscriber with that ID already exists!");
            }
            TableType* CastedTable = static_cast<TableType*>(this);
            this->Subscription = CastedTable->CreateSubscription(Sub);
            return static_cast<SubscribeRet>( this->Subscription );
        }
        /// @brief Gets the subscription stored in this container if it matches a provided ID.
        /// @exception If a subscription with the specified ID is not found, a II_IDENTITY_NOT_FOUND_EXCEPTION will be thrown.
        /// @param ID The unique ID of the subscriber.
        /// @return Returns the subscription with the specified ID, otherwise throws an exception.
        SubscriptionGet GetSubscription(const SubscriberIDType ID) const
        {
            if( this->Subscription->GetID() == ID ) {
                return this->Subscription;
            }
            MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,"No Subscriber with the specified ID was found.");
        }

        /// @brief Gets whether or not this container is holding a valid subscription.
        /// @return Returns true if this container is storing a subscription, false otherwise.
        Boole HasSubscription() const
            { return this->Subscription != nullptr; }

        /// @brief Removes the subscription stored in this container if it matches a provided ID.
        /// @param ID The unique ID of the subscription.
        void Unsubscribe(const SubscriberIDType ID)
        {
            if( this->Subscription->GetID() == ID ) {
                this->Subscription = nullptr;
            }
        }
        /// @brief Removes the subscription from this container.
        /// @return Returns true if a subscription was removed from this container, false otherwise.
        Boole Unsubscribe()
        {
            Boole Removed = this->Subscription != nullptr;
            this->Subscription = nullptr;
            return Removed;
        }
    };//EventSubscriptionContainer

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief This is a container class for the storage of multiple sorted subscribers.
    ///////////////////////////////////////
    template<class TableType, class Traits>
    class EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Unsorted>
    {
    public:
        /// @brief Convenience type for the EventSubscriptionContainer implementation being used.
        using SelfType = EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Unsorted>;
        /// @brief Retrievable type for querying the type of callable interface that is stored by this container.
        using SubscriberType = typename Traits::SubscriberType;
        /// @brief Guaranteed to be a non-pointer, non-reference SubscriberType.
        using SubscriberValue = typename Traits::SubscriberValue;
        /// @brief A pointer to the underlying Subscriber type.
        using SubscriberPtr = typename Traits::SubscriberPtr;
        /// @brief The type to use for uniquely identifying instances of subscribers.
        using SubscriberIDType = typename Traits::SubscriberIDType;
        /// @brief Convenience type for the factory to use when constructing subscriptions.
        using FactoryType = typename Traits::ActualFactoryType;
        /// @brief Convenience type and check for what exactly will be stored by this container.
        using StoredType = typename Traits::StoredType;
        /// @brief Convenience type for the internal container storing the subscriptions.
        using InternalContainerType = std::vector<StoredType>;
        /// @brief Iterator type for subscribers stored by this container.
        using StorageIterator = typename InternalContainerType::iterator;
        /// @brief Const Iterator type for subscribers stored by this container.
        using ConstStorageIterator = typename InternalContainerType::const_iterator;
        /// @brief Convenience type for passing the subscriber as an argument to the Subscribe method.
        using SubscribeArg = typename Traits::SubscribeArg;
        /// @brief Convenience type for the return value of the Subscribe method.
        using SubscribeRet = typename Traits::SubscribeRet;
        /// @brief Convenience type for getting a subscription stored in a table.
        using SubscriptionGet = typename Traits::SubscriptionGet;
    protected:
        /// @brief A container of all the subscriptions tracked by this container.
        InternalContainerType Subscriptions;
    public:
        /// @brief Blank constructor.
        EventSubscriptionContainer() = default;
        /// @brief Copy constructor.
        /// @param Other The other table to NOT be copied.
        EventSubscriptionContainer(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move constructor.
        /// @param Other The other table to be moved.
        EventSubscriptionContainer(EventSubscriptionContainer&& Other) = default;
        /// @brief Class destructor.
        virtual ~EventSubscriptionContainer() = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Operators

        /// @brief Assignment operator.
        /// @param Other The other table to NOT be copied.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move assignment operator.
        /// @param Other The other table to be moved.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(EventSubscriptionContainer&& Other) = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Iterator Access

        /// @brief Gets an iterator to the first subscription in this container.
        /// @return Returns an iterator to the start of the range of stored subscriptions.
        StorageIterator begin()
            { return this->Subscriptions.begin(); }
        /// @brief Gets an iterator to the first subscription in this container.
        /// @return Returns a const iterator to the start of the range of stored subscriptions.
        ConstStorageIterator begin() const
            { return this->Subscriptions.begin(); }
        /// @brief Gets an iterator to one-passed-the-last subscription in this container.
        /// @return Returns a const iterator to the end of the range of stored subscriptions.
        StorageIterator end()
            { return this->Subscriptions.end(); }
        /// @brief Gets an iterator to one-passed-the-last subscription in this container.
        /// @return Returns a const iterator to the end of the range of stored subscriptions.
        ConstStorageIterator end() const
            { return this->Subscriptions.end(); }

        ///////////////////////////////////////////////////////////////////////////////
        // Subscription Management

        /// @brief Adds a subscriber to this container.
        /// @exception If the ID of the Sub is already being used by another subscriber this will throw a II_DUPLICATE_IDENTITY_EXCEPTION.
        /// @param Sub The subscriber to be called when the interested event is fired.
        /// @return Returns an instance of the subscriber storage if applicable.  Could also be void.
        SubscribeRet Subscribe(SubscribeArg Sub)
        {
            for( StorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == EventHelper::ToPointer(Sub)->GetID() ) {
                    MEZZ_EXCEPTION(ExceptionBase::II_DUPLICATE_IDENTITY_EXCEPTION,"A subscriber with that ID already exists!");
                }
            }
            StoredType NewStorage = std::move( FactoryType::CreateSubscription(Sub,static_cast<TableType*>(this)) );
            this->Subscriptions.push_back( NewStorage );
            return NewStorage;
        }
        /// @brief Gets a subscription by the subscriber ID.
        /// @exception If a subscriber with the specified ID is not found, a II_IDENTITY_NOT_FOUND_EXCEPTION will be thrown.
        /// @param ID The unique ID of the subscriber.  Must be unique among the IDs of this container.
        /// @return Returns the subscription with the specified ID, otherwise throws an exception.
        SubscriptionGet GetSubscription(const SubscriberIDType ID) const
        {
            for( ConstStorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == ID ) {
                    return (*SubIt);
                }
            }
            MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,"No Subscriber with the specified ID was found.");
        }

        /// @brief Gets the number of subscribers that have subscribed to the event/object this container is associated with.
        /// @return Returns the number of subscriptions this container is currently tracking.
        Whole GetNumSubscriptions() const
            { return this->Subscriptions.size(); }

        /// @brief Removes a single subscriber from this container.
        /// @param ID The unique ID of the subscriber.  Must be unique among the IDs of this container.
        void Unsubscribe(const SubscriberIDType ID)
        {
            for( StorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == ID ) {
                    this->Subscriptions.erase(SubIt);
                    return;
                }
            }
        }
        /// @brief Removes all subscribers from this container.
        /// @return Returns the number of subscribers removed.
        Whole UnsubscribeAll()
        {
            Whole RemoveCount = this->Subscriptions.size();
            this->Subscriptions.clear();
            return RemoveCount;
        }
    };//EventSubscriptionContainer

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief This is a container class for the storage of multiple sorted subscribers.
    ///////////////////////////////////////
    template<class TableType, class Traits>
    class EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Unsorted_Fixed>
    {
    public:
        /// @brief Convenience type for the EventSubscriptionContainer implementation being used.
        using SelfType = EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Unsorted_Fixed>;
        /// @brief Retrievable type for querying the type of callable interface that is stored by this container.
        using SubscriberType = typename Traits::SubscriberType;
        /// @brief Guaranteed to be a non-pointer, non-reference SubscriberType.
        using SubscriberValue = typename Traits::SubscriberValue;
        /// @brief A pointer to the underlying Subscriber type.
        using SubscriberPtr = typename Traits::SubscriberPtr;
        /// @brief The type to use for uniquely identifying instances of subscribers.
        using SubscriberIDType = typename Traits::SubscriberIDType;
        /// @brief Convenience type for the factory to use when constructing subscriptions.
        using FactoryType = typename Traits::ActualFactoryType;
        /// @brief Convenience type and check for what exactly will be stored by this container.
        using StoredType = typename Traits::StoredType;
        /// @brief Convenience type for the internal container storing the subscriptions.
        using InternalContainerType = ManagedArray<StoredType,Traits::StorageCount>;
        /// @brief Iterator type for subscribers stored by this container.
        using StorageIterator = typename InternalContainerType::iterator;
        /// @brief Const Iterator type for subscribers stored by this container.
        using ConstStorageIterator = typename InternalContainerType::const_iterator;
        /// @brief Convenience type for passing the subscriber as an argument to the Subscribe method.
        using SubscribeArg = typename Traits::SubscribeArg;
        /// @brief Convenience type for the return value of the Subscribe method.
        using SubscribeRet = typename Traits::SubscribeRet;
        /// @brief Convenience type for getting a subscription stored in a table.
        using SubscriptionGet = typename Traits::SubscriptionGet;
    protected:
        /// @brief A container of all the subscriptions tracked by this container.
        InternalContainerType Subscriptions;
    public:
        /// @brief Blank constructor.
        EventSubscriptionContainer() = default;
        /// @brief Copy constructor.
        /// @param Other The other table to NOT be copied.
        EventSubscriptionContainer(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move constructor.
        /// @param Other The other table to be moved.
        EventSubscriptionContainer(EventSubscriptionContainer&& Other) = default;
        /// @brief Class destructor.
        virtual ~EventSubscriptionContainer() = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Operators

        /// @brief Assignment operator.
        /// @param Other The other table to NOT be copied.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move assignment operator.
        /// @param Other The other table to be moved.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(EventSubscriptionContainer&& Other) = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Iterator Access

        /// @brief Gets an iterator to the first subscription in this container.
        /// @return Returns an iterator to the start of the range of stored subscriptions.
        StorageIterator begin()
            { return this->Subscriptions.begin(); }
        /// @brief Gets an iterator to the first subscription in this container.
        /// @return Returns a const iterator to the start of the range of stored subscriptions.
        ConstStorageIterator begin() const
            { return this->Subscriptions.begin(); }
        /// @brief Gets an iterator to one-passed-the-last subscription in this container.
        /// @return Returns a const iterator to the end of the range of stored subscriptions.
        StorageIterator end()
            { return this->Subscriptions.end(); }
        /// @brief Gets an iterator to one-passed-the-last subscription in this container.
        /// @return Returns a const iterator to the end of the range of stored subscriptions.
        ConstStorageIterator end() const
            { return this->Subscriptions.end(); }

        ///////////////////////////////////////////////////////////////////////////////
        // Subscription Management

        /// @brief Adds a subscriber to this container.
        /// @exception If the ID of the Sub is already being used by another subscriber this will throw a II_DUPLICATE_IDENTITY_EXCEPTION.
        /// @param Sub The subscriber to be called when the interested event is fired.
        /// @return Returns an instance of the subscriber storage if applicable.  Could also be void.
        SubscribeRet Subscribe(SubscribeArg Sub)
        {
            for( StorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == EventHelper::ToPointer(Sub)->GetID() ) {
                    MEZZ_EXCEPTION(ExceptionBase::II_DUPLICATE_IDENTITY_EXCEPTION,"A subscriber with that ID already exists!");
                }
            }
            StoredType NewStorage = std::move( FactoryType::CreateSubscription(Sub,static_cast<TableType*>(this)) );
            this->Subscriptions.push_back( NewStorage );
            return NewStorage;
        }
        /// @brief Gets a subscription by the subscriber ID.
        /// @exception If a subscriber with the specified ID is not found, a II_IDENTITY_NOT_FOUND_EXCEPTION will be thrown.
        /// @param ID The unique ID of the subscriber.  Must be unique among the IDs of this container.
        /// @return Returns the subscription with the specified ID, otherwise throws an exception.
        SubscriptionGet GetSubscription(const SubscriberIDType ID) const
        {
            for( ConstStorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == ID ) {
                    return (*SubIt);
                }
            }
            MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,"No Subscriber with the specified ID was found.");
        }

        /// @brief Gets the number of subscribers that have subscribed to the event/object this container is associated with.
        /// @return Returns the number of subscriptions this container is currently tracking.
        Whole GetNumSubscriptions() const
            { return this->Subscriptions.size(); }

        /// @brief Gets the maximum number of subscriptions this container supports.
        /// @return Returns the size of the fixed (stack allocated) storage, and thus the capacity for subscriptions in this container.
        Whole GetMaxSubscriptions() const
            { return this->Subscriptions.capacity(); }

        /// @brief Removes a single subscriber from this container.
        /// @param ID The unique ID of the subscriber.  Must be unique among the IDs of this container.
        void Unsubscribe(const SubscriberIDType ID)
        {
            for( StorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == ID ) {
                    this->Subscriptions.erase(SubIt);
                    return;
                }
            }
        }
        /// @brief Removes all subscribers from this container.
        /// @return Returns the number of subscribers removed.
        Whole UnsubscribeAll()
        {
            Whole RemoveCount = this->Subscriptions.size();
            this->Subscriptions.clear();
            return RemoveCount;
        }
    };//EventSubscriptionContainer

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief This is a container class for the storage of multiple unsorted subscribers.
    ///////////////////////////////////////
    template<class TableType, class Traits>
    class EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Sorted>
    {
    public:
        /// @brief Convenience type for the EventSubscriptionContainer implementation being used.
        using SelfType = EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Sorted>;
        /// @brief Retrievable type for querying the type of callable interface that is stored by this container.
        using SubscriberType = typename Traits::SubscriberType;
        /// @brief Guaranteed to be a non-pointer, non-reference SubscriberType.
        using SubscriberValue = typename Traits::SubscriberValue;
        /// @brief A pointer to the underlying Subscriber type.
        using SubscriberPtr = typename Traits::SubscriberPtr;
        /// @brief The type to use for uniquely identifying instances of subscribers.
        using SubscriberIDType = typename Traits::SubscriberIDType;
        /// @brief Convenience type for the factory to use when constructing subscriptions.
        using FactoryType = typename Traits::ActualFactoryType;
        /// @brief Convenience type and check for what exactly will be stored by this container.
        using StoredType = typename Traits::StoredType;
        /// @brief The function to use for sorting this container.
        using StoragePredicate = typename Traits::StoragePredicate;
        /// @brief Convenience type for the internal container storing the subscriptions.
        using InternalContainerType = SortedVector<StoredType,StoragePredicate>;
        /// @brief Iterator type for subscribers stored by this container.
        using StorageIterator = typename InternalContainerType::iterator;
        /// @brief Const Iterator type for subscribers stored by this container.
        using ConstStorageIterator = typename InternalContainerType::const_iterator;
        /// @brief Convenience type for passing the subscriber as an argument to the Subscribe method.
        using SubscribeArg = typename Traits::SubscribeArg;
        /// @brief Convenience type for the return value of the Subscribe method.
        using SubscribeRet = typename Traits::SubscribeRet;
        /// @brief Convenience type for getting a subscription stored in a table.
        using SubscriptionGet = typename Traits::SubscriptionGet;
    protected:
        /// @brief A container of all the subscriptions tracked by this container.
        InternalContainerType Subscriptions;
    public:
        /// @brief Blank constructor.
        EventSubscriptionContainer() = default;
        /// @brief Copy constructor.
        /// @param Other The other table to NOT be copied.
        EventSubscriptionContainer(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move constructor.
        /// @param Other The other table to be moved.
        EventSubscriptionContainer(EventSubscriptionContainer&& Other) = default;
        /// @brief Class destructor.
        virtual ~EventSubscriptionContainer() = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Operators

        /// @brief Assignment operator.
        /// @param Other The other table to NOT be copied.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move assignment operator.
        /// @param Other The other table to be moved.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(EventSubscriptionContainer&& Other) = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Iterator Access

        /// @brief Gets an iterator to the first subscription in this container.
        /// @return Returns an iterator to the start of the range of stored subscriptions.
        StorageIterator begin()
            { return this->Subscriptions.begin(); }
        /// @brief Gets an iterator to the first subscription in this container.
        /// @return Returns a const iterator to the start of the range of stored subscriptions.
        ConstStorageIterator begin() const
            { return this->Subscriptions.begin(); }
        /// @brief Gets an iterator to one-passed-the-last subscription in this container.
        /// @return Returns a const iterator to the end of the range of stored subscriptions.
        StorageIterator end()
            { return this->Subscriptions.end(); }
        /// @brief Gets an iterator to one-passed-the-last subscription in this container.
        /// @return Returns a const iterator to the end of the range of stored subscriptions.
        ConstStorageIterator end() const
            { return this->Subscriptions.end(); }

        ///////////////////////////////////////////////////////////////////////////////
        // Subscription Management

        /// @brief Adds a subscriber to this container.
        /// @exception If the ID of the Sub is already being used by another subscriber this will throw a II_DUPLICATE_IDENTITY_EXCEPTION.
        /// @param Sub The subscriber to be called when the interested event is fired.
        /// @return Returns an instance of the subscriber storage if applicable.  Could also be void.
        SubscribeRet Subscribe(SubscribeArg Sub)
        {
            if( this->Subscriptions.end() != this->Subscriptions.find( EventHelper::ToPointer(Sub)->GetID() ) ) {
                MEZZ_EXCEPTION(ExceptionBase::II_DUPLICATE_IDENTITY_EXCEPTION,"A subscriber with that ID already exists!");
            }
            StoredType NewStorage = std::move( FactoryType::CreateSubscription(Sub,static_cast<TableType*>(this)) );
            this->Subscriptions.add( NewStorage );
            return NewStorage;
        }
        /// @brief Gets a subscription by the subscriber ID.
        /// @exception If a subscriber with the specified ID is not found, a II_IDENTITY_NOT_FOUND_EXCEPTION will be thrown.
        /// @param ID The unique ID of the subscriber.  Must be unique among the IDs of this container.
        /// @return Returns the subscription with the specified ID, otherwise throws an exception.
        SubscriptionGet GetSubscription(const SubscriberIDType ID) const
        {
            for( ConstStorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == ID ) {
                    return (*SubIt);
                }
            }
            MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,"No Subscriber with the specified ID was found.");
        }

        /// @brief Gets the number of subscribers that have subscribed to the event/object this container is associated with.
        /// @return Returns the number of subscriptions this container is currently tracking.
        Whole GetNumSubscriptions() const
            { return this->Subscriptions.size(); }

        /// @brief Removes a single subscriber from this container.
        /// @param ID The unique ID of the subscriber.  Must be unique among the IDs of this container.
        void Unsubscribe(const SubscriberIDType ID)
        {
            for( StorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == ID ) {
                    this->Subscriptions.erase(SubIt);
                    return;
                }
            }
        }
        /// @brief Removes all subscribers from this container.
        /// @return Returns the number of subscribers removed.
        Whole UnsubscribeAll()
        {
            Whole RemoveCount = this->Subscriptions.size();
            this->Subscriptions.clear();
            return RemoveCount;
        }
    };//EventSubscriptionContainer

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief This is a container class for the storage of multiple sorted subscribers.
    ///////////////////////////////////////
    template<class TableType, class Traits>
    class EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Sorted_Fixed>
    {
    public:
        /// @brief Convenience type for the EventSubscriptionContainer implementation being used.
        using SelfType = EventSubscriptionContainer<TableType,Traits,SubscriptionContainerType::SCT_Sorted_Fixed>;
        /// @brief Retrievable type for querying the type of callable interface that is stored by this container.
        using SubscriberType = typename Traits::SubscriberType;
        /// @brief Guaranteed to be a non-pointer, non-reference SubscriberType.
        using SubscriberValue = typename Traits::SubscriberValue;
        /// @brief A pointer to the underlying Subscriber type.
        using SubscriberPtr = typename Traits::SubscriberPtr;
        /// @brief The type to use for uniquely identifying instances of subscribers.
        using SubscriberIDType = typename Traits::SubscriberIDType;
        /// @brief Convenience type for the factory to use when constructing subscriptions.
        using FactoryType = typename Traits::ActualFactoryType;
        /// @brief Convenience type and check for what exactly will be stored by this container.
        using StoredType = typename Traits::StoredType;
        /// @brief The function to use for sorting this container.
        using StoragePredicate = typename Traits::StoragePredicate;
        /// @brief Convenience type for the internal container storing the subscriptions.
        using InternalContainerType = SortedManagedArray<StoredType,Traits::StorageCount,StoragePredicate>;
        /// @brief Iterator type for subscribers stored by this container.
        using StorageIterator = typename InternalContainerType::iterator;
        /// @brief Const Iterator type for subscribers stored by this container.
        using ConstStorageIterator = typename InternalContainerType::const_iterator;
        /// @brief Convenience type for passing the subscriber as an argument to the Subscribe method.
        using SubscribeArg = typename Traits::SubscribeArg;
        /// @brief Convenience type for the return value of the Subscribe method.
        using SubscribeRet = typename Traits::SubscribeRet;
        /// @brief Convenience type for getting a subscription stored in a table.
        using SubscriptionGet = typename Traits::SubscriptionGet;
    protected:
        /// @brief A container of all the subscriptions tracked by this container.
        InternalContainerType Subscriptions;
    public:
        /// @brief Blank constructor.
        EventSubscriptionContainer() = default;
        /// @brief Copy constructor.
        /// @param Other The other table to NOT be copied.
        EventSubscriptionContainer(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move constructor.
        /// @param Other The other table to be moved.
        EventSubscriptionContainer(EventSubscriptionContainer&& Other) = default;
        /// @brief Class destructor.
        virtual ~EventSubscriptionContainer() = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Operators

        /// @brief Assignment operator.
        /// @param Other The other table to NOT be copied.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(const EventSubscriptionContainer& Other) = delete;
        /// @brief Move assignment operator.
        /// @param Other The other table to be moved.
        /// @return Returns a reference to this.
        EventSubscriptionContainer& operator=(EventSubscriptionContainer&& Other) = default;

        ///////////////////////////////////////////////////////////////////////////////
        // Iterator Access

        /// @brief Gets an iterator to the first subscription in this container.
        /// @return Returns an iterator to the start of the range of stored subscriptions.
        StorageIterator begin()
            { return this->Subscriptions.begin(); }
        /// @brief Gets an iterator to the first subscription in this container.
        /// @return Returns a const iterator to the start of the range of stored subscriptions.
        ConstStorageIterator begin() const
            { return this->Subscriptions.begin(); }
        /// @brief Gets an iterator to one-passed-the-last subscription in this container.
        /// @return Returns a const iterator to the end of the range of stored subscriptions.
        StorageIterator end()
            { return this->Subscriptions.end(); }
        /// @brief Gets an iterator to one-passed-the-last subscription in this container.
        /// @return Returns a const iterator to the end of the range of stored subscriptions.
        ConstStorageIterator end() const
            { return this->Subscriptions.end(); }

        ///////////////////////////////////////////////////////////////////////////////
        // Subscription Management

        /// @brief Adds a subscriber to this container.
        /// @exception If the ID of the Sub is already being used by another subscriber this will throw a II_DUPLICATE_IDENTITY_EXCEPTION.
        /// @param Sub The subscriber to be called when the interested event is fired.
        /// @return Returns an instance of the subscriber storage if applicable.  Could also be void.
        SubscribeRet Subscribe(SubscribeArg Sub)
        {
            if( this->Subscriptions.end() != this->Subscriptions.find( EventHelper::ToPointer(Sub)->GetID() ) ) {
                MEZZ_EXCEPTION(ExceptionBase::II_DUPLICATE_IDENTITY_EXCEPTION,"A subscriber with that ID already exists!");
            }
            StoredType NewStorage = std::move( FactoryType::CreateSubscription(Sub,static_cast<TableType*>(this)) );
            this->Subscriptions.add( NewStorage );
            return NewStorage;
        }
        /// @brief Gets a subscription by the subscriber ID.
        /// @exception If a subscriber with the specified ID is not found, a II_IDENTITY_NOT_FOUND_EXCEPTION will be thrown.
        /// @param ID The unique ID of the subscriber.  Must be unique among the IDs of this container.
        /// @return Returns the subscription with the specified ID, otherwise throws an exception.
        SubscriptionGet GetSubscription(const SubscriberIDType ID) const
        {
            for( ConstStorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == ID ) {
                    return (*SubIt);
                }
            }
            MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,"No Subscriber with the specified ID was found.");
        }

        /// @brief Gets the number of subscribers that have subscribed to the event/object this container is associated with.
        /// @return Returns the number of subscriptions this container is currently tracking.
        Whole GetNumSubscriptions() const
            { return this->Subscriptions.size(); }

        /// @brief Gets the maximum number of subscriptions this container supports.
        /// @return Returns the size of the fixed (stack allocated) storage, and thus the capacity for subscriptions in this container.
        Whole GetMaxSubscriptions() const
            { return this->Subscriptions.capacity(); }

        /// @brief Removes a single subscriber from this container.
        /// @param ID The unique ID of the subscriber.  Must be unique among the IDs of this container.
        void Unsubscribe(const SubscriberIDType ID)
        {
            for( StorageIterator SubIt = this->Subscriptions.begin() ; SubIt != this->Subscriptions.end() ; ++SubIt )
            {
                if( EventHelper::ToPointer( *SubIt )->GetID() == ID ) {
                    this->Subscriptions.erase(SubIt);
                    return;
                }
            }
        }
        /// @brief Removes all subscribers from this container.
        /// @return Returns the number of subscribers removed.
        Whole UnsubscribeAll()
        {
            Whole RemoveCount = this->Subscriptions.size();
            this->Subscriptions.clear();
            return RemoveCount;
        }
    };//EventSubscriptionContainer

    /// @}
}//Mezzanine

#endif
