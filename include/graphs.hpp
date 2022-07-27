#include <exception>
#include <numeric>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <algorithm>
#include <utility>
#include <optional>
#include <cassert>

namespace my
{

struct basic_node
{
  public:
    using id_type = std::size_t;

    basic_node(id_type id, std::vector<id_type>&& neighbours);
    basic_node(id_type id, std::vector<id_type> const& neighbours);

    id_type id() const noexcept;
    id_type& id() noexcept;

    bool has_neighbour(id_type nid) const;
    bool add_neighbour(id_type nid);
    bool remove_neighbour(id_type nid);
    void clear_neightbours() noexcept;

    std::vector<id_type> const& neighbours() const noexcept;

  protected:
    std::vector<id_type>::iterator get_neighbour(id_type nid);
    std::vector<id_type>::const_iterator get_neighbour(id_type nid) const;

  private:
    id_type id_;
    std::vector<id_type> neighbours_; // Remains sorted
};

class child_node : public basic_node
{
  public:
    using id_type = basic_node::id_type;

    child_node(id_type id, std::vector<id_type>&& neighbours);
    child_node(id_type id, std::vector<id_type> const& neighbours);

    std::optional<id_type> const& parent() const noexcept;
    std::optional<id_type>& parent() noexcept;

  private:
    std::optional<id_type> parent_;
};

template <typename TNode = basic_node>
class graph
{
  public:
    graph() = default;

    void reserve(std::size_t n)
    {
        nodes_.reserve(n);
    }

    TNode& add_node(std::vector<typename TNode::id_type>&& neighbours)
    {
        const std::size_t id = nodes_.empty() ? 0 : nodes_.back().id() + 1;
        nodes_.emplace_back(id, std::forward<std::vector<typename TNode::id_type>>(neighbours));
        return nodes_.back();
    }

    void pop_node(std::vector<typename TNode::id_type> const& neighbours)
    {
        std::size_t nid = nodes_.back().id();
        nodes_.pop_back(neighbours);
        std::for_each(nodes_.begin(), nodes_.end(), [](auto& n) { n.remove_neighbour(nid); });
    }

    void add_edge(typename TNode::id_type from, typename TNode::id_type to, bool bidirectional = true)
    {
        nodes_[from].add_neighbour(to);
        if (bidirectional) {
            nodes_[to].add_neighbour(from);
        }
    }

    auto const& nodes() const noexcept
    {
        return nodes_;
    }

    [[nodiscard]] TNode const& node(typename TNode::id_type id) const noexcept
    {
        assert(id < nodes_.size());
        return nodes_[id];
    }

    [[nodiscard]] TNode& node(typename TNode::id_type id) noexcept
    {
        assert(id < nodes_.size());
        return nodes_[id];
    }

  private:
    std::vector<TNode> nodes_;
};

template <typename T>
[[nodiscard]] std::tuple<typename T::id_type, typename T::id_type, bool> random_edge(graph<T> const& graph)
{
    // Number of edges
    const auto nedges = std::transform_reduce(
      graph.nodes().begin(), graph.nodes().end(), 0, [&](auto const& node) { return node.neighbours().size(); },
      std::plus<std::size_t>{});

    if (nedges == 0) {
        return {0, 0, false};
    }

    // Random edge id
    std::random_device rd{};
    std::uniform_int_distribution<std::size_t> dist{0, nedges - 1u};
    const std::size_t selected_edge = dist(rd);

    // Finding edge
    std::size_t i = 0;
    for (auto const& node : graph.nodes()) {
        const auto nedges = node.neighbours().size();
        if (i + nedges > selected_edge) {
            std::size_t offset = selected_edge - i;
            return std::make_tuple(node.id(), node.neighbours()[offset], true);
        }
        i += nedges;
    }

    throw std::logic_error("random_edge_if: edge not found");
}

template <typename T>
void contract(graph<T>& graph, typename T::id_type from, typename T::id_type to)
{
    auto new_node = graph.add_node({from, to});
    graph.node(from).parent() = std::make_optional(new_node.id());
    graph.node(to).parent() = std::make_optional(new_node.id());

    graph.node(from).clear_neightbours();
    graph.node(to).clear_neightbours();

    std::for_each(graph.nodes().begin(), graph.nodes().end(), [&](auto& node) {
        if (node.remove_neighbour(from)) {
            node.add_neighbour(new_node.id());
        }
        if (node.remove_neighbour(to)) {
            node.add_neighbour(new_node.id());
        }
    });
}

inline void minimum_cut(graph<child_node>& graph)
{
    auto [left, right, valid] = random_edge(graph);
    // No edges
    if (!valid) {
        return;
    }

    contract(graph, left, right);
}

} // namespace my