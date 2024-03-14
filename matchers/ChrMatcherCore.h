#pragma once

#include "../include/faststring.h"
#include "../include/PointerChain.h"

// All matchers must be a part of this namespace
namespace ChrMatcher {
	// This is the base matcher class. All modifiers must derive from it.
	class Matcher {
	protected:
		Matcher() {}

	public:
		virtual ~Matcher() {}

		bool match(void* ChrIns) { if (!!ChrIns) return this->onMatch(ChrIns); }

	private:
		// Override this function while preserving its signature to create custom matchers.
		virtual bool onMatch(void* ChrIns) = 0;
	};
}

#include "BaseMatchers.h"