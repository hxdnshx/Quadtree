#include <quadtree.h>
#include <catch.h>

#define COMMON_TAGS "[unit][quadtree]"
#define COMMON_TEXT "unit text"

using cv::Point2d;

TEST_CASE("init inner" COMMON_TEXT, COMMON_TAGS)
{
	quadtree<> root(cv::Rect2d(0, 0, 2, 2));
	root.init_inner();
	int top = 0;
	int bottom = 2;
	int left = 0;
	int right = 2;
	int center = 1;

	REQUIRE(root.child(0)->area.tl() == Point2d(center, top));
	REQUIRE(root.child(0)->area.br() == Point2d(right, center));
	REQUIRE(root.child(1)->area.tl() == Point2d(left, top));
	REQUIRE(root.child(1)->area.br() == Point2d(center, center));
	REQUIRE(root.child(2)->area.tl() == Point2d(center, center));
	REQUIRE(root.child(2)->area.br() == Point2d(right, bottom));
	REQUIRE(root.child(3)->area.tl() == Point2d(left, center));
	REQUIRE(root.child(3)->area.br() == Point2d(center, bottom));
}

TEST_CASE("parent" COMMON_TEXT, COMMON_TAGS)
{
	quadtree<> root(cv::Rect2d(0, 0, 2, 2));
	root.init_inner();

	REQUIRE(root.is_root());
	REQUIRE(root.is_have_child());
	REQUIRE(root.child(0)->get_root() == &root);
	REQUIRE(root.child(1)->get_root() == &root);
	REQUIRE(root.child(2)->get_root() == &root);
	REQUIRE(root.child(3)->get_root() == &root);
	REQUIRE(root.child(0)->get_depth() == 1);
	REQUIRE(root.child(1)->get_depth() == 1);
	REQUIRE(root.child(2)->get_depth() == 1);
	REQUIRE(root.child(3)->get_depth() == 1);
}

TEST_CASE("get index" COMMON_TEXT, COMMON_TAGS)
{
	quadtree<> root(cv::Rect2d(0, 0, 1, 1));
	root.insert_block(0.4, 0.4, 1, false);

	REQUIRE(root.is_have_child());
	REQUIRE(root.get_side(0.4, 0.4)->get_loc() == 1);
	REQUIRE(root.get_side(0.6, 0.4)->get_loc() == 0);
	REQUIRE(root.get_side(0.4, 0.6)->get_loc() == 3);
	REQUIRE(root.get_side(0.6, 0.6)->get_loc() == 2);
}