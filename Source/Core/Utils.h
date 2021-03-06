#pragma once

namespace Utils
{
	template<class T>
	uint8_t CountSetBits(T aVal);

	bool IsNan(glm::vec3 aVec);
	bool IsNan(glm::vec4 aVec);

	// Groups copies of elements of TColl by TPred condition
	template<class TAssocColl, class TColl, class TPred>
	TAssocColl GroupBy(const TColl& aColl, TPred aPred);
}

template<class T>
uint8_t Utils::CountSetBits(T aVal)
{
	static_assert(std::is_integral_v<T>, "T mush be integral!");
	uint8_t bitCount = 0;
	while (aVal)
	{
		bitCount += aVal & 1;
		aVal >>= 1;
	}
	return bitCount;
}

template<class TAssocColl, class TColl, class TPred>
TAssocColl Utils::GroupBy(const TColl& aColl, TPred aPred)
{
	TAssocColl assocCollection;
	for (const auto& item : aColl)
	{
		auto key = aPred(item);
		// TODO: generalize the inserter, probably via splitting TAssocColl
		// and using if constexpr(std::is_same_v)
		assocCollection[key].push_back(item);
	}
	return assocCollection;
}