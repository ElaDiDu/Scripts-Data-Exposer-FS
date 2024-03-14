#pragma once

namespace ChrMatcher {
	class All : public Matcher {
		virtual bool onMatch(void* ChrIns) { return true; }
	};

	class Player : public Matcher {
	public:
		Player(bool matchAllPlayers = false) : matchAll(matchAllPlayers) {}
		const bool matchAll;

	private:
		virtual bool onMatch(void* ChrIns) 
		{ 
			uint64_t maskedHandle = reinterpret_cast<uint64_t*>(ChrIns)[1] ^ 0xFFFFFFFF15A00000ull; 
			return this->matchAll && maskedHandle <= 0xFFull || !maskedHandle; 
		}
	};

	class Torrent : public Matcher {
	public:
		Torrent(bool matchAllTorrents = false) : matchAll(matchAllTorrents) {}
		const bool matchAll;

	private:
		virtual bool onMatch(void* ChrIns)
		{
			uint64_t maskedHandle = reinterpret_cast<uint64_t*>(ChrIns)[1] ^ 0xFFFFFFFF15C00000ull;
			return this->matchAll && maskedHandle <= 0xFFull || !maskedHandle;
		}
	};

	template <std::size_t N> class Map : public Matcher {
	public:
		Map(const wchar_t(&name)[N]) : name(name) {}
		const wchar_t (&name)[N];

	private:
		virtual bool onMatch(void* ChrIns) 
		{
			char* pMapName = PointerChain::make<char>(ChrIns, 0x190, 0x0, 0x60, 0x18u, 0x0u).get();
			return !!pMapName && strcmp_fast(pMapName, this->name);
		}
	};

	template <std::size_t N> class Name : public Matcher {
	public:
		Name(const wchar_t(&name)[N]) : name(name) {}
		const wchar_t(&name)[N];

	private:
		virtual bool onMatch(void* ChrIns)
		{
			char* pChrName = PointerChain::make<char>(ChrIns, 0x190, 0x0, 0x28, 0x0u, 0x0u).get();
			return !!pChrName && strcmp_fast(pChrName, this->name);
		}
	};

	template <std::size_t N> class Model : public Matcher {
	public:
		Model(const wchar_t(&name)[N]) : name(name) {}
		const wchar_t(&name)[N];

	private:
		virtual bool onMatch(void* ChrIns)
		{
			char* pModelName = PointerChain::make<char>(ChrIns, 0x28, 0xA8u).get();
			return !!pModelName && strcmp_fast(pModelName, this->name);
		}
	};

	class EntityID : public Matcher {
	public:
		EntityID(int entityID) : ID(entityID) {}
		const int ID;

	private:
		virtual bool onMatch(void* ChrIns)
		{
			int entityID = *PointerChain::make<int>(ChrIns, 0x1E8);
			return entityID == this->ID;
		}
	};

	class EntityGroupID : public Matcher {
	public:
		EntityGroupID(int entityGroupID) : ID(entityGroupID) {}
		const int ID;

	private:
		virtual bool onMatch(void* ChrIns)
		{
			__m128i* entityGroupID = PointerChain::make<__m128i>(ChrIns, 0x190, 0x0, 0x28, 0x60u, 0x1C).get();
			if (!entityGroupID) return false;

			__m128i ID = _mm_set1_epi32(this->ID);
			__m128i first = _mm_loadu_si128(entityGroupID);
			__m128i second = _mm_loadu_si128(entityGroupID + 1);
			first = _mm_cmpeq_epi32(first, ID);
			second = _mm_cmpeq_epi32(second, ID);
			first = _mm_or_si128(first, second);

			return !_mm_testz_si128(first, first);
		}
	};

	class NPCParamID : public Matcher {
	public:
		NPCParamID(int paramID) : ID(paramID) {}
		const int ID;

	private:
		virtual bool onMatch(void* ChrIns)
		{
			int* pParamID = PointerChain::make<int>(ChrIns, 0x190, 0x0, 0x28, 0x68u, 0x4).get();
			return !!pParamID && *pParamID == this->ID;
		}
	};

	class ThinkParamID : public Matcher {
	public:
		ThinkParamID(int paramID) : ID(paramID) {}
		const int ID;

	private:
		virtual bool onMatch(void* ChrIns)
		{
			int* pParamID = PointerChain::make<int>(ChrIns, 0x190, 0x0, 0x28, 0x68u, 0x8).get();
			return !!pParamID && *pParamID == this->ID;
		}
	};
}