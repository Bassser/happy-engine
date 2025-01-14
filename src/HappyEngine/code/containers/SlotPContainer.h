//HappyEngine Copyright (C) 2011 - 2014  Evil Interactive
//
//This file is part of HappyEngine.
//
//    HappyEngine is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    HappyEngine is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with HappyEngine.  If not, see <http://www.gnu.org/licenses/>.
//
//Author:  Bastian Damman
//Created: 27/11/2011

#ifndef _HE_SLOT_P_CONTAINER_H_
#define _HE_SLOT_P_CONTAINER_H_
#pragma once

namespace he {

template<typename T>
class SlotPContainer
{
public:
    SlotPContainer() {}
    ~SlotPContainer() {}
    //default copy OK

    size_t insert(const T& obj)
    {
        if (m_FreeSlots.size() > 0)
        {
            size_t id(m_FreeSlots.front());
            m_FreeSlots.pop();
            m_MemPool[id] = obj;
            return id;
        }
        else
        {
            size_t id(m_MemPool.size());
            m_MemPool.add(obj);
            return id;
        }
    }
    const T& get(size_t id)
    {
        HE_ASSERT(id < m_MemPool.size(), "id is not in a valid range");
        HE_ASSERT(m_MemPool[id] != nullptr, "no object @id");
        return m_MemPool[id];
    }
    T remove(size_t id)
    {
        HE_ASSERT(id < m_MemPool.size(), "id is not in a valid range");

        m_FreeSlots.push(id);

        T& pObj(m_MemPool[id]);
        m_MemPool[id] = nullptr;

        return pObj;
    }
    size_t getId(const T& obj)
    {
        HE_ASSERT(obj != nullptr, "object == nullptr");

        for (size_t i(0); i < m_MemPool.size(); ++i)
            if (m_MemPool[i] == obj)
                return i;
        LOG(LogType_ProgrammerAssert, "object not present in container");
        return 0;
    }

    T& operator[](size_t id)
    {
        HE_ASSERT(id < m_MemPool.size(), "id is not in a valid range");
        HE_ASSERT(m_MemPool[id] != nullptr, "no object @id");
        return m_MemPool[id];
    }
    const T& operator[](size_t id) const
    {
        HE_ASSERT(id < m_MemPool.size(), "id is not in a valid range");
        HE_ASSERT(m_MemPool[id] != nullptr, "no object @id");
        return m_MemPool[id];
    }

    template<typename R>
    class const_iterator
    {
    public: typedef typename he::ObjectList<R>::const_iterator _iterator;
    private:
        _iterator m_It;
        _iterator m_EndIt;

    public:
        const_iterator(const _iterator& iterator, const _iterator& end):
          m_It(iterator), m_EndIt(end) { }

        const_iterator& operator++()
        {
            do
            {
                ++m_It;
            } while (m_It != m_EndIt && (*m_It) == nullptr);

            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator ret(*this);
            ++ret;
            return ret;
        }
        bool operator==(const const_iterator& it)
        {
            return (m_It == it.m_It);
        }
        bool operator!=(const const_iterator& it)
        {
            return (m_It != it.m_It);
        }
        const R& operator*()
        {
            return *m_It;
        }

    };

    const_iterator<T> cbegin()
    {
        return const_iterator<T>(m_MemPool.cbegin(), m_MemPool.cend());
    }
    const_iterator<T> cend()
    {
        return const_iterator<T>(m_MemPool.cend(), m_MemPool.cend());
    }

    size_t size()
    {
        return m_MemPool.size() - m_FreeSlots.size();
    }

private:
    
    he::ObjectList<T> m_MemPool;
    he::Queue<size_t> m_FreeSlots;

    //disable default assignment operator
    SlotPContainer& operator=(const SlotPContainer&);
};


template<class _InIt, class _Fn1> inline
_Fn1 for_each(_InIt _First, _InIt _Last, _Fn1 _Func)
{	// perform function for each element
    for (; _First != _Last; ++_First)
        _Func(*_First);
    return (_Func);
}

} //end namespace

#endif
