#ifndef PHOTO_CPP
#define PHOTO_CPP

#include <set>
#include <string>

class Photo
{
public:
	int id;
	bool horizontal;
	std::set<std::string> tags;
	int tags_count;
	bool used;

	Photo(){}

	Photo(int id, bool horizontal, std::set<std::string>& tags)
	{
		this->id = id;
		this->horizontal = horizontal;
		this->tags = tags;
		this->tags_count = tags.size();
		this->used = false;
	}

	bool has_tag(std::string& tag)
	{
		return tags.count(tag);
	}
};

#endif