#include<iostream>
template <class Callback>
int CollectFeatures(Callback CB)
{
	int count = 0;
	for (int i = 0; i < 11; i++)
	{
		if (CB(i))
		{
			count++;
		}
	}
	return count;
}
bool AddFeature(size_t Feature)
{
	return Feature % 2;
}
int main()
{
	int i = CollectFeatures([](size_t Feature) -> bool { return AddFeature(Feature); });
	std::cout << i << std::endl;
}