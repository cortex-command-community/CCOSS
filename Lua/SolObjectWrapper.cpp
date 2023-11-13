// Make sure that this wrapper file is always set to NOT use pre-compiled headers and conformance mode (/permissive) otherwise everything will be on fire cause luabind is a nightmare!

#include "SolObjectWrapper.h"

#include "sol/sol.hpp"

namespace RTE {

// With multithreaded Lua, objects can be destructed from multiple threads at once
// This is okay, but Sol wants to do some management on the lua state when one of it's objects is deleted
// This means that potentially an object being deleted by one lua state actually exists in another lua state
// And upon deletion, it's unsafe for Sol to poke at the state until we're out the multithreaded context
// As such, we don't actually delete the object until we're in a safe environment outside the multithreaded parts
// Note - this is required even though we force objects in multithreaded environments to be within our Sol state
// This is because we may assign an object to another state in a singlethreaded context, before the GC runs in the multithreaded context
static std::vector<sol::object *> s_QueuedDeletions;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SolObjectWrapper::ApplyQueuedDeletions() {
	for (sol::object *obj : s_QueuedDeletions) {
		delete obj;
	}

	s_QueuedDeletions.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SolObjectWrapper::~SolObjectWrapper() {
	if (m_OwnsObject) {
		static std::mutex mut;
		std::lock_guard<std::mutex> guard(mut);

		s_QueuedDeletions.push_back(m_SolObject);
	}
}

}