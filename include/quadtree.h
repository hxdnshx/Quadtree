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
template <class InnerType = int>
class quadtree
{
public:
	using ThisType = quadtree<InnerType>;
private:
	ThisType* _child[4]{ nullptr,nullptr ,nullptr ,nullptr };
	ThisType* _parent{ nullptr };
	bool _is_block{ false };
public:
	quadtree();
	~quadtree();
	quadtree(const cv::Rect2d& rect);
	template<class... Args>
	quadtree(const cv::Rect2d& rect, Args&&... params);
	void init_inner(function<ThisType*(ThisType*, const cv::Rect2d&)> custom_ctor = nullptr);

	/*
	┌─┬─┐
	│ 1│ 0│
	├─┼─┤
	│ 3│ 2│
	└─┴─┘
	*/
	inline ThisType* child(int index) const { return _child[index]; };
	ThisType* parent() const { return _parent; }
	InnerType data;
	inline bool is_have_child() const { return _child[0] != nullptr || _child[1] != nullptr || _child[2] != nullptr || _child[3] != nullptr;};
	inline bool is_block() const { return _is_block; }
	inline bool is_root() const { return _parent == nullptr; }
	cv::Rect2d area;
	void insert_block(double x, double y, int max_depth, bool is_block = true, function<ThisType*(ThisType*,const cv::Rect2d&)> custom_ctor = nullptr);
	ThisType* find(double x, double y);
	int get_index(double x, double y) const;
	ThisType* get_side(double x, double y);
	ThisType* get_root() const;
	void enum_neighbor(function<void(ThisType*)> callback) const;
	int get_depth() const;
	int get_loc() const;
};

#endif // QUADTREE_H

