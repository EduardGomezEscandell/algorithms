#include <graphs.hpp>

namespace my
{

basic_node::basic_node(id_type id, std::vector<id_type>&& neighbours) : id_{id}, neighbours_{std::move(neighbours)}
{
    std::sort(neighbours_.begin(), neighbours_.end());
}

basic_node::basic_node(id_type id, std::vector<id_type> const& neighbours) : id_{id}, neighbours_{neighbours}
{
    std::sort(neighbours_.begin(), neighbours_.end());
}

child_node::id_type basic_node::id() const noexcept
{
    return id_;
}

child_node::id_type& basic_node::id() noexcept
{
    return id_;
}

bool basic_node::add_neighbour(id_type nid)
{
    const auto it = get_neighbour(nid);
    if (it == neighbours_.end()) {
        neighbours_.push_back(nid);
        return true;
    }
    if (*it == nid) {
        return false;
    }
    neighbours_.insert(it, nid);
    return true;
}

bool basic_node::has_neighbour(id_type nid) const
{
    const auto it = get_neighbour(nid);
    return it != neighbours_.end() && *it == nid;
}

bool basic_node::remove_neighbour(id_type nid)
{
    const auto it = get_neighbour(nid);
    if (it == neighbours_.end()) {
        return false;
    }
    neighbours_.erase(it);
    return true;
}

void basic_node::clear_neightbours() noexcept
{
    neighbours_.clear();
}

std::vector<basic_node::id_type> const& basic_node::neighbours() const noexcept
{
    return neighbours_;
}

std::vector<child_node::id_type>::iterator basic_node::get_neighbour(id_type nid)
{
    return std::partition_point(neighbours_.begin(), neighbours_.end(), [=](auto x) noexcept { return x < nid; });
}

std::vector<child_node::id_type>::const_iterator basic_node::get_neighbour(id_type nid) const
{
    return std::partition_point(neighbours_.cbegin(), neighbours_.cend(), [=](auto x) noexcept { return x < nid; });
}

child_node::child_node(id_type id, std::vector<id_type>&& neighbours) :
    basic_node(id, std::forward<std::vector<id_type>>(neighbours))
{
}
child_node::child_node(id_type id, std::vector<id_type> const& neighbours) : basic_node(id, neighbours)
{
}

std::optional<child_node::id_type> const& child_node::parent() const noexcept
{
    return parent_;
}

std::optional<child_node::id_type>& child_node::parent() noexcept
{
    return parent_;
}

} // namespace my