#include <iostream>
#include <vector>
#include <tuple>
#include "photo.cpp"
#include "slide.cpp"
#include "evaluator.cpp"
#include <fstream>

Photo read_photo(int id)
{
	char horizontal_or_vertical;
	std::cin >> horizontal_or_vertical;
	bool horizontal = horizontal_or_vertical == 'H';
	int tags_count;
	std::cin >> tags_count;
	tags_set tags = tags_set();
	for(int i = 0; i < tags_count; i++)
	{
		std::string tag;
		std::cin >> tag;
		tags.insert(tag);
	}
	return Photo(id, horizontal, tags);
}

auto next_vertical(int i, std::vector<Photo>& photos)
{
	while(photos[i].horizontal)
	{
		i++;
	}
	return std::make_tuple(i+1, photos[i]);
}

auto next_horizontal(int i, std::vector<Photo>& photos)
{
	while(!photos[i].horizontal)
	{
		i++;
	}
	return std::make_tuple(i+1, photos[i]);
}

auto max_transition_next_unused_slide(Slide& slide, std::vector<Slide*>& slides)
{
	int max_transition_score = 0;
	int max_transition_slide_index = 0;

	for(int i = 0; i < slides.size(); i++)
	{
		if(slides[i]->used)
		{
			continue;
		}
		int slide_score = Evaluator::get_transition_score(slide, *(slides[i]));
		if(slide_score >= max_transition_score)
		{
			max_transition_score = slide_score;
			max_transition_slide_index = i;
		}
	}
	return std::make_tuple(max_transition_slide_index, max_transition_score);
}

auto max_transition_next_unused_photo(Slide& slide, std::vector<Photo>& photos)
{
	int max_transition_score = 0;
	int max_transition_photo_index = 0;

	for(int i = 0; i < photos.size(); i++)
	{
		if(photos[i].used)
		{
			continue;
		}
		int transition_score = Evaluator::get_transition_score(slide, photos[i]);
		if(transition_score >= max_transition_score)
		{
			max_transition_score = transition_score;
			max_transition_photo_index = i;
		}
	}
	return std::make_tuple(max_transition_photo_index, max_transition_score);
}

auto max_transition_next_unused_photo(Slide& slide, Photo& photo1, std::vector<Photo>& photos)
{
	int max_transition_score = 0;
	int max_transition_photo_index = 0;

	for(int i = 0; i < photos.size(); i++)
	{
		if(photos[i].used || photos[i].horizontal)
		{
			continue;
		}
		int transition_score = Evaluator::get_transition_score(slide, photo1, photos[i]);
		if(transition_score >= max_transition_score)
		{
			max_transition_score = transition_score;
			max_transition_photo_index = i;
		}
	}
	return std::make_tuple(max_transition_photo_index, max_transition_score);
}

void write_log(std::ofstream& ofstream, std::string str)
{
	ofstream << str;
}

struct Counter
{
	int horizontal_photos_count = 0;
	int vertical_photos_count = 0;
};

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);

	std::ofstream log_file;
	log_file.open("log_photo.txt");

	Counter counter;
	int photos_count;
	std::cin >> photos_count;
	std::vector<Photo> photos = std::vector<Photo>(photos_count);
	for(int i = 0; i < photos_count; i++)
	{
		photos[i] = read_photo(i);
		if(photos[i].horizontal)
		{
			counter.horizontal_photos_count++;
		}
		else
		{
			counter.vertical_photos_count++;
		}
	}

	/*std::cout << "horizontal photos: " << counter.horizontal_photos_count << std::endl;
	std::cout << "vertical photos: " << counter.vertical_photos_count << std::endl;*/

	int horizontal_slides_count = counter.horizontal_photos_count;
	int vertical_slides_count = counter.vertical_photos_count / 2;
	int slides_count = horizontal_slides_count + vertical_slides_count;

	std::vector<Slide> slideshow = std::vector<Slide>(slides_count);

	if(photos[0].horizontal)
	{
		std::vector<Photo> slide_photos = std::vector<Photo>(1);
		slide_photos[0] = photos[0];
		photos[0].used = true;
		slideshow[0] = Slide(slide_photos);
	}
	else
	{
		std::vector<Photo> slide_photos = std::vector<Photo>(2);
		slide_photos[0] = photos[0];
		photos[0].used = true;
		int next_vertical_index;
		Photo next_photo;
		std::tie(next_vertical_index, next_photo) = next_vertical(1, photos);
		slide_photos[1] = photos[next_vertical_index-1];
		photos[next_vertical_index-1].used = true;
		slideshow[0] = Slide(slide_photos);
	}

	for(int i = 0; i < slideshow.size() - 1; i++)
	{
		int photo_score;
		int photo_index;
		std::tie(photo_index, photo_score) = max_transition_next_unused_photo(slideshow[i], photos);
		if(photos[photo_index].horizontal)
		{
			std::vector<Photo> slide_photos = std::vector<Photo>(1);
			slide_photos[0] = photos[photo_index];
			photos[photo_index].used = true;
			slideshow[i+1] = Slide(slide_photos);
			log_file << "slideshow index: " << i+1 << ", added photo index: " << photo_index << ", added slide score: " << photo_score << std::endl;
		}
		else
		{
			std::vector<Photo> slide_photos = std::vector<Photo>(2);
			slide_photos[0] = photos[photo_index];
			photos[photo_index].used = true;
			int photo2_score;
			int photo2_index;
			std::tie(photo2_index, photo2_score) = max_transition_next_unused_photo(slideshow[i], photos[photo_index], photos);
			slide_photos[1] = photos[photo2_index];
			photos[photo2_index].used = true;
			slideshow[i+1] = Slide(slide_photos);
			log_file << "slideshow index: " << i+1 << ", added photo indexes: " << photo_index << " " << photo2_index << ", added photo score " << photo_score << " " << photo2_score << std::endl;
		}
	}


	log_file.close();

	std::cout << "Slideshow size: " << slideshow.size() << std::endl;
	std::cout << "Slideshow score: " << Evaluator::get_slideshow_score(slideshow) << std::endl;
	std::cout << "Slideshow check: " << Evaluator::check(slideshow) << std::endl;
	std::cout << std::endl;

	std::cout << slideshow.size() << std::endl;
	for(Slide slide: slideshow)
	{
		std::cout << slide.to_string() << std::endl;
	}

}