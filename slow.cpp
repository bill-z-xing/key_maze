
constexpr bool debug = false;

template <class T>
vector<vector<T>> build_2d_vector(int num_rows, int num_cols, T val)
{
    vector<vector<T>> retval(num_rows);
    
    for (auto & inner : retval)
    {
        inner.resize(num_cols, val);
    }
    
    return std::move(retval);
}



struct Cell
{ 
    bool is_valid() const
    {
        return _row >= 0;    
    }
    
    friend std::ostream & operator<<(std::ostream & os, Cell cell)
    {
        os << "(" << cell._row << "," << cell._col <<  ")";
        return os;
    }
    
    int _row = -1;
    int _col = -1;
};

constexpr size_t kNumLetters = 'Z' - 'A' + 1;
constexpr size_t kNumLettersSmall = 'z' - 'a' + 1;
static_assert(kNumLetters == kNumLettersSmall, "kNumLetters != kNumLettersSmall");

class Grid
{
    
    
public:
    Grid(const vector<string> & grid)
    :
        _grid(grid)
    {
        _num_rows = grid.size();
        assert(_num_rows > 0);
            
        _num_cols = grid[0].size();
        assert(_num_cols > 0);
            
        for (const auto & inner : grid)
        {
            assert(_num_cols == static_cast<int>(inner.size()));
        }
            
        int num_locks = 0;
            
        iterate_over_all_cells(
            [&](Cell cell)
            {
                assert(is_legal(cell));
                
                if (is_lock(cell))
                {
                    size_t i = get_lock_index(get_char(cell));
                    _lock_cells[i] = cell;
                    ++num_locks;
                    
                }
                
                if (is_key(cell))
                {
                    size_t i = get_key_index(get_char(cell));
                    _key_cells[i] = cell;
                    ++_num_keys;
                }
                
                if (is_start(cell))
                {
                    assert(!_start.is_valid());
                    _start = cell;
                }
            }
            );
            
        assert(_start.is_valid());
        assert(num_locks == _num_keys);
    }
    
    Cell get_start_cell() const
    {
        return _start;
    }
    
    
    bool is_start(Cell cell) const
    {
        char c = get_char(cell);
        return c == '@';
    }
    
    bool is_wall(Cell cell) const
    {
        char c = get_char(cell);
        return c == '#';
        
    }
    
    bool is_lock(Cell cell) const
    {
        char c = get_char(cell);
        return is_lock(c);
    }
    
    static bool is_lock(char c)
    {
        return c >= 'A' && c <= 'Z';
    }
    
    bool is_key(Cell cell) const
    {
        char c = get_char(cell);
        return is_key(c);
    }
    
    static bool is_key(char c)
    {
        return c >= 'a' && c <= 'z';
    }
    
    bool is_free(Cell cell) const
    {
        char c = get_char(cell);
        return c == '.';
    }
    
    bool is_legal(Cell cell) const
    {
        char c = get_char(cell);
        return is_start(cell) || is_wall(cell) || is_lock(cell) || is_key(cell) || is_free(cell);
    }
    
    char get_char(Cell cell) const
    {
        return _grid[cell._row][cell._col];
    }
    
    Cell find_key_for_lock(Cell cell) const
    {
        assert(is_lock(cell));
        char c = get_char(cell);
        size_t offset = get_lock_index(c);
            
        return _key_cells[offset];
    }
    
    template <class F>
    void iterate_over_all_cells(F f)
    {
        for (int irow = 0; irow < _num_rows; ++irow)
        {
            for (int icol = 0; icol < _num_cols; ++icol)
            {
                Cell cell{irow, icol};
                f(cell);
            }
        }
    }
    
    static size_t get_lock_index(char c)
    {
        assert(is_lock(c));
        return c - 'A';
    }
    
    static size_t get_key_index(char c)
    {
        assert(is_key(c));
        return c - 'a';
    }
    
    size_t get_lock_index(Cell cell) const
    {
        return get_lock_index( get_char(cell) );
    }
    
    size_t get_key_index(Cell cell) const
    {
        return get_key_index( get_char(cell) );
    }
    
    int num_rows() const
    {
        return _num_rows;
    }
    
    int num_cols() const
    {
        return _num_cols;
    }
    
    int num_keys() const
    {
        return _num_keys;
    }
    
    int num_locks() const
    {
        return num_keys();
    }


private:
    const std::vector<std::string> & _grid;
    int _num_rows = 0;
    int _num_cols = 0;
    Cell _start;
    
    std::array<Cell, kNumLetters> _lock_cells;
    std::array<Cell, kNumLetters> _key_cells;
    
    int _num_keys = 0;
};

enum Dir
{
    kDirBegin = 0, kDirLeft = 0, kDirRight, kDirUp, kDirDown, kDirMax
};

class NeighbourIter
{
public:

    
    NeighbourIter() = delete;
    
    NeighbourIter & operator++()
    {
        _dir = (Dir)((int)_dir + 1);
        return *this;
    }
    
    friend bool operator!=(NeighbourIter a, NeighbourIter b)
    {
        return a._dir != b._dir;
    }
    
    Cell operator*() const
    {
        switch (_dir)
        {
        case 0:
        {
            Cell cell = _cell;
            --cell._row;
            if (cell._row < 0)
            {
                return Cell();
            }
            return cell;
        }
                
        case 1:
        {
            Cell cell = _cell;
            ++cell._row;
            if (cell._row >= _grid.num_rows())
            {
                return Cell();
            }
            return cell;
        }
                
        case 2:
        {
            Cell cell = _cell;
            --cell._col;
            if (cell._col < 0)
            {
                return Cell();
            }
            return cell;
        }
                
        case 3:
        {
            Cell cell = _cell;
            ++cell._col;
            if (cell._col >= _grid.num_cols())
            {
                return Cell();
            }
            return cell;
        }
            
        default:
            assert(0);            
        }
    }
    
    Dir get_dir() const
    {
        return _dir;
    }

private:  
    
    NeighbourIter(const Grid & grid, Cell cell, Dir dir)
    :
          _grid(grid)
        , _cell(cell)
        , _dir(dir)
    {}
    
    const Grid & _grid;
    Cell _cell;
    Dir _dir = kDirMax;
    
public:
    friend NeighbourIter begin_neighbour(const Grid & grid, Cell cell);    
    friend NeighbourIter end_neighbour(const Grid & grid, Cell cell);

};

NeighbourIter begin_neighbour(const Grid & grid, Cell cell)
{
    return NeighbourIter(grid, cell, kDirBegin);
}

NeighbourIter end_neighbour(const Grid & grid, Cell cell)
{
    return NeighbourIter(grid, cell, kDirMax);
}




class Path
{
public:
    Path(const Grid & grid)
    :
          _grid(grid)
        , _key_hit_counters(grid.num_keys())
    {
        for (auto & inner : _visited_directional)
        {
            inner = build_2d_vector(grid.num_rows(), grid.num_cols(), false);
        }
    }
    
    void visit(Cell cell, Dir dir)
    {
        _path_vec.emplace_back(cell, dir);
        
        if (dir != kDirMax)
        {
            assert(_visited_directional[dir][cell._row][cell._col] == false);
            _visited_directional[dir][cell._row][cell._col] = true;
        }
        
        if (_grid.is_key(cell))
        {
            size_t key_idx = _grid.get_key_index(cell);
            int prev_count = _key_hit_counters[key_idx]++;
            assert(prev_count >= 0);
            if (prev_count == 0)
            {
                ++_num_keys_acquired;
            }
        }
    }
    
    void unvisit()
    {
        assert(!_path_vec.empty());
        
        Cell cell; Dir dir;
        
        std::tie(cell, dir) = _path_vec.back();
        _path_vec.pop_back();
        
        // Ignore start visit
        if (dir != kDirMax)
        {
            assert(_visited_directional[dir][cell._row][cell._col] == true);        
            _visited_directional[dir][cell._row][cell._col] = false;
        }
        
        if (_grid.is_key(cell))
        {
            size_t key_index = _grid.get_key_index(cell);
            int prev_count = _key_hit_counters[key_index]--;
            assert(prev_count > 0);
            if (prev_count == 1)
            {
                --_num_keys_acquired;
            }
        }
    }
    
    int num_steps() const
    {
        return _path_vec.size();
    }
    
    bool visited(Cell cell, Dir dir) const
    {
        return _visited_directional[dir][cell._row][cell._col];
    }
    
    bool key_was_acquired_for_lock(Cell cell) const
    {
        assert(_grid.is_lock(cell));
        size_t key_index = _grid.get_lock_index(cell);
                
        return _key_hit_counters[key_index] > 0;
    }
    
    bool complete() const
    {
        return _num_keys_acquired == _grid.num_keys();
    }
    
    friend bool operator<(const Path & a, const Path & b)
    {
        return a.num_steps() < b.num_steps();
    }
        
private:
    const Grid & _grid;
    std::vector<std::pair<Cell, Dir>> _path_vec;
    std::vector<vector<bool>> _visited_directional[kDirMax];
    std::vector<int> _key_hit_counters;
    int _num_keys_acquired = 0;
};


class Solution 
{
public:
    int shortestPathAllKeys(std::vector<std::string> & arg_grid) 
    {
        Grid grid(arg_grid);
        Path path(grid);
        
        recurse_visit_next_cell(grid, path, grid.get_start_cell(), kDirMax);
        
        if (!_best_solution)
        {
            return -1;
        }
        else
        {
            return _best_solution->num_steps() - 1;
        }
        
    }
    
    void recurse_visit_next_cell(const Grid & grid, Path & path, Cell parent_cell, Dir dir)
    {
        if (debug) std::cout << "Visit " << parent_cell << std::endl;
        path.visit(parent_cell, dir);
        
        perform_this_visit(grid, path, parent_cell);
        
        path.unvisit();
        if (debug) std::cout << "Unvisit " << parent_cell << std::endl;
    }
    
    void perform_this_visit(const Grid & grid, Path & path, Cell parent_cell)
    {
        if (path.complete())
        {
            // Taken enough locks. Stop here.
            // Register this path, if it's better than any existing
            
            if (!_best_solution || path < *_best_solution)
            {
                if (debug) std::cout << "Found best path" << std::endl;
                _best_solution = std::make_unique<Path>(path);
            }
            
            if (debug) std::cout << "Path complete" << std::endl;
            return;
        }

        const auto end_iter = end_neighbour(grid, parent_cell);
        for (auto iter = begin_neighbour(grid, parent_cell); iter != end_iter; ++iter)
        {                
            Cell child_cell = *iter;
            if (child_cell.is_valid())
            {
                examine_child(grid, path, child_cell, iter.get_dir());
            }
        }
    }
    
    void examine_child(const Grid & grid, Path & path, Cell child_cell, Dir dir)
    {
        if (path.visited(child_cell, dir)) // Wrong. Correction: Never revisit a node in the same direction.
        {
            // Dead end because it's going into a cycle
            if (debug) std::cout << "Skip child " << child_cell << " due to visited" << std::endl;
            return;
        }
        
        if (grid.is_wall(child_cell))
        {
            // Dead end because it's a wall
            if (debug) std::cout << "Skip child " << child_cell << " due to wall" << std::endl;
            return;
        }
        
        if (grid.is_lock(child_cell) && !path.key_was_acquired_for_lock(child_cell))
        {
            // Dead end because lock hasn't been unlocked by key
            if (debug) std::cout << "Skip child " << child_cell << " due to lock" << std::endl;
            return;
        }
                
        recurse_visit_next_cell(grid, path, child_cell, dir);
    }
    
private:
    std::unique_ptr<Path> _best_solution;
    
};
