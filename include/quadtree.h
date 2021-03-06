#ifndef QUADTREE_H
#define QUADTREE_H
#include <opencv2/core.hpp>
#include <functional>
#include <list>
#include <stack>


using std::function;

struct dir_table
{
	/**
	template<class InnerType>
	* \brief 方向，需要注意的是，这里direction - 5即为这个斜方向在quadtree<InnerType>::child中的index了
	*/
	enum direction { HALT = 0, R = 1, L = 2, D = 3, U = 4, RU = 5, LU = 6, RD = 7, LD = 8 };
	direction dir;
	int loc;
	static const dir_table table[9][4];
	static direction reverseDir(direction dir)
	{
		static direction rev[] = { HALT,L,R,U,D,LD,RD,LU,RU };
		return rev[static_cast<int>(dir)];
	}
};


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
	InnerType data;
	cv::Rect2d area;

	quadtree(){};

	~quadtree()
	{
		if (is_have_child())
		{
			for (int i = 0; i < 4; i++)
			{
				delete _child[i];
				_child[i] = nullptr;
			}
		}
	}

	quadtree(const cv::Rect2d& rect) :area(rect){}

	template<class... Args>
	quadtree(const cv::Rect2d& rect, Args&&... params) : data(std::forward(params)), area(rect){}

	void init_inner(function<ThisType*(ThisType*, const cv::Rect2d&)> custom_ctor = nullptr)
	{
		for (int i = 0; i < 4; i++) {
			cv::Rect2d area;
			cv::Point2d&& tl = this->area.tl();
			cv::Point2d&& br = this->area.br();
			const cv::Point2d ct = (tl + br) * 0.5;
			switch (i)
			{
			case 0:
				area = cv::Rect2d(cv::Point2d(ct.x, tl.y), cv::Point2d(br.x, ct.y));
				break;
			case 1:
				area = cv::Rect2d(tl, ct);
				break;
			case 2:
				area = cv::Rect2d(ct, br);
				break;
			case 3:
				area = cv::Rect2d(cv::Point2d(tl.x, ct.y), cv::Point2d(ct.x, br.y));
				break;
			default:;
			}
			if (custom_ctor != nullptr)
			{
				_child[i] = custom_ctor(this, area);
			}
			else
			{
				_child[i] = new ThisType(area);
				_child[i]->_parent = this;
			}
		}
	}

	/*
	┌─┬─┐
	│ 1│ 0│
	├─┼─┤
	│ 3│ 2│
	└─┴─┘
	*/
	inline ThisType* child(int index) const { return _child[index]; };
	ThisType* parent() const { return _parent; }
	inline bool is_have_child() const { return _child[0] != nullptr || _child[1] != nullptr || _child[2] != nullptr || _child[3] != nullptr;};
	inline bool is_block() const { return _is_block; }
	inline bool is_root() const { return _parent == nullptr; }

	void insert_block(double x, double y, int max_depth, bool is_block = true, function<ThisType*(ThisType*,const cv::Rect2d&)> custom_ctor = nullptr)
	{
		int depth = max_depth - get_depth();
		quadtree* current = this;
		for (;;)
		{
			if (depth == 0)
			{
				current->_is_block = is_block;
				break;
			}
			auto inner_node = current->get_side(x, y);
			if (inner_node == nullptr)
			{
				current->init_inner(custom_ctor);
			}
			current = current->_child[current->get_index(x, y)];
			depth -= 1;
		}
	}

	ThisType* find(double x, double y) const
	{
		if (!area.contains(cv::Point2d(x, y)))
			return nullptr;
		auto current = this;
		for (;;)
		{
			auto child = current->get_side(x, y);
			if (child == nullptr)
				break;
			current = child;
		}
		return current;
	}

	int get_index(double x, double y) const
	{
		const auto center = (area.br() + area.tl()) * 0.5;
		return ((y < center.y) ? 0 : 2) + ((x > center.x) ? 0 : 1);
	}

	ThisType* get_side(double x, double y)
	{
		const int index = get_index(x, y);
		return _child[index];
	}

	ThisType* get_root() const
	{
		auto current = _parent;
		for (;;)
		{
			if (current->_parent == nullptr)
				return current;
			current = current->_parent;
		}
	}
	
	void enum_neighbor(function<void(ThisType*)> callback) const
	{
		if (callback == nullptr)
			return;
		std::list<quadtree*> results;
		struct context
		{
			std::stack<int> inner_loc;
			int relative_depth = 0;
			dir_table::direction dir;
			dir_table::direction orig_dir;
			quadtree* current;
		};
		std::stack<context> pending;
		for (int i = 1; i <= 8; i++)
		{
			context ctx;
			ctx.dir = ctx.orig_dir = static_cast<dir_table::direction>(i);
			ctx.current = this;
			pending.push(ctx);
		}
		for (;;)
		{
			if (pending.empty())
				break;
			auto& ctx = pending.top();
			if (ctx.dir == dir_table::HALT)
			{
				for (;;)
				{
					if (ctx.relative_depth == 0)
						break;
					if (!ctx.current->has_child())
						break;
					ctx.current = ctx.current->child[ctx.inner_loc.top()];
					ctx.inner_loc.pop();
					--ctx.relative_depth;
				}
				if (ctx.relative_depth != 0)
				{
					results.push_back(ctx.current);
				}
				else
				{
					if (ctx.orig_dir <= 4)
					{
						const auto dir = dir_table::reverseDir(ctx.orig_dir);
						std::stack<quadtree*> pending_enum;
						pending_enum.push(ctx.current);
						for (;;)
						{
							if (pending_enum.empty())
								break;
							const auto curr = pending_enum.top();
							pending_enum.pop();
							if (curr != nullptr)
							{
								if (curr->has_child())
								{
									static int child_table[4][2] = { { 2,0 },{ 3,1 },{ 3,2 },{ 1,0 } };
									pending_enum.emplace((curr->child[child_table[dir - 1][0]]));
									pending_enum.emplace((curr->child[child_table[dir - 1][1]]));
								}
								else
									results.push_back(curr);
							}
						}
					}
					else
					{
						const auto dir = dir_table::reverseDir(ctx.orig_dir);
						auto curr = ctx.current;
						for (;;)
						{
							if (curr->has_child())
								curr = curr->child[dir - 5];
							else
							{
								results.push_back(curr);
								break;
							}
						}
					}
				}
				pending.pop();
			}
			else
			{
				if (ctx.current->_parent == nullptr)
				{
					//已经到根节点，也就是说找不到这个方向的相邻节点
					pending.pop();
				}
				else
				{
					int loc = ctx.current->get_loc();
					ctx.inner_loc.push(dir_table::table[ctx.dir][loc].loc);
					++ctx.relative_depth;
					ctx.dir = dir_table::table[ctx.dir][loc].dir;
					ctx.current = ctx.current->_parent;
				}
			}
		}
		results.unique();
		for (auto result : results)
			callback(result);
	}

	int get_depth() const
	{
		int level = 0;
		auto current = this;
		for (;;)
		{
			if (current->_parent == nullptr)
				return level;
			level++;
			current = current->_parent;
		}
	}

	int get_loc() const
	{
		if (this->_parent == nullptr)
			return -1;
		for (int i = 0; i < 4; i++)
		{
			if (this->_parent->_child[i] == this)
				return i;
		}
		return -1;
	}
};

#endif // QUADTREE_H

