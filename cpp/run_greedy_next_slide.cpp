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
	log_file.open("log.txt");

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

	int horizontal_slides_count = counter.horizontal_photos_count;
	int vertical_slides_count = counter.vertical_photos_count / 2;
	int slides_count = horizontal_slides_count + vertical_slides_count;

	std::vector<Slide> horizontal_slides = std::vector<Slide>(horizontal_slides_count);
	std::vector<Slide> vertical_slides = std::vector<Slide>(vertical_slides_count);

	int photos_horizontal_index = 0;
	int photos_vertical_index = 0;
	for(int i = 0; i < horizontal_slides_count; i++)
	{
		std::vector<Photo> slide_photos = std::vector<Photo>(1);
		std::tie(photos_horizontal_index, slide_photos[0]) = next_horizontal(photos_horizontal_index, photos);
		Slide slide = Slide(slide_photos);
		horizontal_slides[i] = slide;
	}
	for(int i = 0; i < vertical_slides_count; i++)
	{
		std::vector<Photo> slide_photos = std::vector<Photo>(2);
		std::tie(photos_vertical_index, slide_photos[0]) = next_vertical(photos_vertical_index, photos);
		std::tie(photos_vertical_index, slide_photos[1]) = next_vertical(photos_vertical_index, photos);
		Slide slide = Slide(slide_photos);
		vertical_slides[i] = slide;
	}

	std::vector<Slide*> slides = std::vector<Slide*>(slides_count);
	for(int i = 0; i < horizontal_slides_count; i++)
	{
		slides[i] = &horizontal_slides[i];
	}
	for(int i = 0; i < vertical_slides_count; i++)
	{
		slides[horizontal_slides_count + i] = &vertical_slides[i];
	}

	std::vector<Slide*> slideshow = std::vector<Slide*>(slides_count);
	slideshow[0] = slides[0];
	slides[0]->used = true;


	for(int i = 0; i < slideshow.size() - 1; i++)
	{
		int slide_score;
		int slide_index;
		std::tie(slide_index, slide_score) = max_transition_next_unused_slide(*(slideshow[i]), slides);
		slideshow[i+1] = slides[slide_index];
		slides[slide_index]->used = true;
		// std::cout << "Slideshow index: " << i << std::endl;
		// std::cout << "Added slide index: " << slide_index << std::endl;
		// std::cout << "Added slide score: " << slide_score << std::endl;

		// log_file << "slideshow index: " << i << ", added slide index: " << slide_index << ", added slide score: " << slide_score << std::endl; 
		log_file << "slideshow index: " << i+1 << ", added slide index: " << slide_index << ", added slide score: " << slide_score << std::endl; 
	}

	log_file.close();

	std::cout << "Slideshow size: " << slideshow.size() << std::endl;
	std::cout << "Slideshow score: " << Evaluator::get_slideshow_score(slideshow) << std::endl;

	std::cout << slideshow.size() << std::endl;
	for(Slide* slide: slideshow)
	{
		std::cout << slide->to_string() << std::endl;
	}

}