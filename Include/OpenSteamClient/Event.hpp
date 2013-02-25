#ifndef OPENSTEAMCLIENT_EVENT_HPP
#define OPENSTEAMCLIENT_EVENT_HPP

#include <list>
#include <memory>
#include <functional>

namespace Sc
{
	using std::list;
	using std::function;
	using std::shared_ptr;
	
	template<class T>
	class Event
	{
	public:
		typedef function<void(const T &)> Handler;
		typedef function<void(const T &)> *HandlerId;

		HandlerId Add(Handler fn);
		void Remove(HandlerId id);
		void Call(const T &t) const;
	private:
		list<shared_ptr<Handler>> m_handlers;
	};

	template<>
	class Event<void>
	{
	public:
		typedef function<void()> Handler;
		typedef function<void()> *HandlerId;

		HandlerId Add(Handler fn);
		void Remove(HandlerId id);
		void Call() const;
	private:
		list<shared_ptr<Handler>> m_handlers;
	};

	// ------------------------------------------------------------------------
	// Event<T> Implementation
	// ------------------------------------------------------------------------

	template<class T>
	inline typename Event<T>::HandlerId Event<T>::Add(Handler fn)
	{
		auto id = new Handler(fn);
		m_handlers.push_back(shared_ptr<Handler>(id));
		return id;
	}

	template<class T>
	inline void Event<T>::Remove(HandlerId id)
	{
		for(auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
		{
			if(it->get() == id)
			{
				m_handlers.erase(it);
				break;
			}
		}
	}

	template<class T>
	inline void Event<T>::Call(const T &t) const
	{
		for(auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
		{
			(**it)(t);
		}
	}

	// ------------------------------------------------------------------------
	// Event<void> Implementation
	// ------------------------------------------------------------------------

	inline Event<void>::HandlerId Event<void>::Add(Handler fn)
	{
		auto id = new Handler(fn);
		m_handlers.push_back(shared_ptr<Handler>(id));
		return id;
	}

	inline void Event<void>::Remove(HandlerId id)
	{
		for(auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
		{
			if(it->get() == id)
			{
				m_handlers.erase(it);
				break;
			}
		}
	}

	inline void Event<void>::Call() const
	{
		for(auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
		{
			(**it)();
		}
	}
}

#endif