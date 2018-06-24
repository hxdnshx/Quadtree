#ifndef QUADTREE_H
#define QUADTREE_H
#include <opencv2/core.hpp>
#include <functional>
#include <vector>

using std::function;

/**
 * \brief 一个在y轴负方向，x轴正方向坐标系下的四叉树实现
 * \tparam InnerType 内部存储的数据类型
 */
template <class InnerType>
class quadtree
{
private:
	quadtree* _child[4];
	quadtree* _parent{ nullptr };
	bool _is_block{ false };
public:
	quadtree(const cv::Rect2d rect);
	/*
	┌─┬─┐
	│ 1│ 0│
	├─┼─┤
	│ 3│ 2│
	└─┴─┘
	*/
	quadtree& child(int index);
	quadtree& parent() { return *_parent; }
	InnerType data;
	inline quadtree& operator [](int index);
	bool is_have_child();
	bool is_block() { return _is_block; }
	cv::Rect2d area;
	void insertblock(double x, double y, int max_depth, function<void(quadtree*)> new_node_cb = nullptr);
	int get_index(double x, double y);
	quadtree& get_side(double x, double y);
	quadtree& get_root() const;
	void enum_neighbor(function<void(quadtree*)> callback);
	int get_level() const;
	int get_loc() const;
};

#endif // QUADTREE_H

