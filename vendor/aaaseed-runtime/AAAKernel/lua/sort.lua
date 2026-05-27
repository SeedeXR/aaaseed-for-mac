aaa.show_file_begin( "sort" )

local function build_sort()
	-- This is the LuaJIT implementation of Smoothsort [1], a comparison-based
	-- sorting algorithm with worst-case asymptotic O(n log n) behaviour, best-case
	-- O(n) behaviour, and a smooth transition in between. Largely based on the C++
	-- code by Keith Schwarz [2], translated to LuaJIT by Lesley De Cruz.

	-- [1] Dijkstra, E. W. (1982). Smoothsort, an alternative for sorting in situ.
	-- Science of Computer Programming, 1(3), 223-233.
	-- [2] Schwarz, K. Smoothsort demystified. http://www.keithschwarz.com/smoothsort/.

	local ffi = require("ffi")
	local n_leonardo_numbers = 46
	local heap_shape_t = ffi.typeof([[
	  struct {int32_t min_size; bool flags[?]; }
	]])

	--[[
	local abi32 = true
	local leonardo_numbers, heap_shape
	if ffi.abi("32bit") then
	  n_leonardo_numbers = 46
	  leonardo_numbers = ffi.new("int32_t[?]", n_leonardo_numbers, 1, 1, 3, 5, 9, 15, 25, 41, 67, 109, 177, 287, 465, 753, 1219, 1973, 3193, 5167, 8361, 13529, 21891, 35421, 57313, 92735, 150049, 242785, 392835, 635621, 1028457, 1664079, 2692537, 4356617, 7049155, 11405773, 18454929, 29860703, 48315633, 78176337, 126491971, 204668309, 331160281, 535828591, 866988873, 1402817465, 2269806339, 3672623805)
	  heap_shape = ffi.new("int32_t[?]",n_leonardo_numbers)
	else
	  abi32 = false
	  n_leonardo_numbers = 92
	  leonardo_numbers = ffi.new("int64_t[?]", n_leonardo_numbers, 1, 1, 3, 5, 9, 15, 25, 41, 67, 109, 177, 287, 465, 753, 1219, 1973, 3193, 5167, 8361, 13529, 21891, 35421, 57313, 92735, 150049, 242785, 392835, 635621, 1028457, 1664079, 2692537, 4356617, 7049155, 11405773, 18454929, 29860703, 48315633, 78176337, 126491971, 204668309, 331160281, 535828591, 866988873, 1402817465, 2269806339, 3672623805, 5942430145, 9615053951, 15557484097, 25172538049, 40730022147, 65902560197, 106632582345, 172535142543, 279167724889, 451702867433, 730870592323, 1182573459757, 1913444052081, 3096017511839, 5009461563921, 8105479075761, 13114940639683, 21220419715445, 34335360355129, 55555780070575, 89891140425705, 145446920496281, 235338060921987, 380784981418269, 616123042340257, 996908023758527, 1613031066098785, 2609939089857313, 4222970155956099, 6832909245813413, 11055879401769513, 17888788647582927, 28944668049352441, 46833456696935369, 75778124746287811, 122611581443223181, 198389706189510993, 321001287632734175, 519390993822245169, 840392281454979345, 1359783275277224515, 2200175556732203861, 3559958832009428377, 5760134388741632239, 9320093220751060617, 15080227609492692857)
	  heap_shape = ffi.new("int64_t[?]",n_leonardo_numbers)
	end --]]

	-- NB don't use unsigned types (cfr. http://lua-users.org/lists/lua-l/2011-03/msg00518.html)
	local leonardo_numbers = ffi.new("int32_t[?]", n_leonardo_numbers, 1, 1, 3, 5, 9, 15, 25, 41, 67, 109, 177, 287, 465, 753, 1219, 1973, 3193, 5167, 8361, 13529, 21891, 35421, 57313, 92735, 150049, 242785, 392835, 635621, 1028457, 1664079, 2692537, 4356617, 7049155, 11405773, 18454929, 29860703, 48315633, 78176337, 126491971, 204668309, 331160281, 535828591, 866988873, 1402817465, 2269806339, 3672623805)

	--[[
	local leonardo_number
	do
	  local lcache  = {[0]=1,1}
	  leonardo_number = function(i)
	    local lnum = lcache[i]
	    if not lnum then
	      lnum = leonardo_numbers[i-1] + leonardo_numbers[i-2] + 1
	      lcache[i] = lnum
	    end
	    return lnum
	  end
	end
	--]]


	-- The second child is stored immediately to the left of the root.
	-- The first child of an L(s)-sized tree is stored L(s-2) elements to the left
	-- of the second child.
	--
	-- E.g. An L(4)-sized tree (Lt_4) has its second child at root-1 and its first
	-- child L(4-2) = L(2) = 3 positions to the left of the second child.
	--
	--   1 2 3 4 5 6 7 8 9=root
	--                   o
	--            . - ' /
	--           o     o
	--        .'/   .'/
	--       o o   o o
	--    .'/
	--   o o

	local function child2(root)
	  return root-1
	end

	local function child1(root,size)
	  return child2(root) - leonardo_numbers[size-2]
	end

	local function larger_child(lheap,root,size,comp)
	  local first,second = child1(root,size), child2(root)
	  return comp(lheap[first],lheap[second]) and second or first
	end

	-- Rebalance a Leonardo tree by bubbling down ("sifting") an element inserted
	-- at the root node: swap it with its largest child until its largest child no
	-- longer exceeds the element.
	local function rebalance_tree(lheap,root,size,comp)
	  while (size>1) do
	    local lc = larger_child(lheap,root,size,comp)
	    size = (lc==child2(root,size)) and size-2 or size-1
	    -- we've found a child which is not larger than the root.
	    if not comp(lheap[root], lheap[lc]) then return end
	    -- otherwise, bubble down by swapping with the largest child node.
	    lheap[root],lheap[lc]=lheap[lc],lheap[root]
	    root = lc
	  end
	end

	-- Readjust the heap of leonardo trees by pseudo-insertion-sorting the new
	-- element: first insertion sort ("trinkle") the element to the left until
	-- swapping is no longer allowed, then bubble down ("trinkle") the element.
	-- Swapping the root of the smaller tree (i.e. the new element) with the one of
	-- the preceding, larger tree is allowed ONLY if the root of the larger tree is
	-- bigger than the new element AS WELL AS the children of the root of the
	-- smaller tree; otherwise, serveral tree-rebalance operations would be
	-- triggered. If we stop there, all we need to do is bubble down the new
	-- element into the tree it's reached so far.
	local function rectify_lheap(lheap,heap_shape,init,cur_tree_root,cur_tree_size,comp)
	  -- do this until we've reached the last tree:
	  local next_tree_root = cur_tree_root - leonardo_numbers[cur_tree_size] -- get next root
	  while next_tree_root > init-1 do -- >0 if 1-indexed; >=0 if 0-indexed
	    local tocompare = cur_tree_root -- the new element
	    -- not Lt_0 or Lt_1: compare with the largest child if it's larger than the new element
	    if cur_tree_size>1 then
	      local lc = larger_child(lheap,cur_tree_root,cur_tree_size,comp)
	      -- the root of the preceding tree should be compared to
	      -- max(the new element, its current largest child)
	      if comp(lheap[cur_tree_root],lheap[lc]) then
	        tocompare = lc
	      end
	    end
	    -- break if new element is larger than the next heap root OR its largest child.
	    if not comp(lheap[tocompare],lheap[next_tree_root]) then break end
	    -- otherwise, we can go ahead and swap
	    lheap[cur_tree_root],lheap[next_tree_root] = lheap[next_tree_root],lheap[cur_tree_root]
	    repeat cur_tree_size = cur_tree_size + 1
	    until heap_shape.flags[cur_tree_size]
	    cur_tree_root,next_tree_root = next_tree_root, next_tree_root - leonardo_numbers[cur_tree_size] -- get next root
	  end
	  rebalance_tree(lheap,cur_tree_root,cur_tree_size,comp)
	end

	local function push(lheap,heap_shape,init,last,len,comp)
	  -- Now we're going to push the (last+1)th item on the heap while we try to
	  -- satisfy the following two invariants:
	  -- 1. each tree satisfies the max-heap property (rebalance_tree)
	  -- 2. the root nodes of all of our trees are sorted (rectify_lheap; this
	  -- also takes care of 1.)
	  -- However, we're going to take care of 2. in a lazy way, which means that
	  -- we're going to check whether our tree has reached its final size before
	  -- actually doing a rectify_lheap on it (making it "trusty").
	  local rectify = false
	  --Case 1: If the last two heaps have sizes that differ by one, we
	  --add the new element by merging the last two heaps
	  if heap_shape.min_size>=0 and heap_shape.flags[heap_shape.min_size+1] then
	    heap_shape.flags[heap_shape.min_size] = false
	    heap_shape.flags[heap_shape.min_size+1] = false
	    heap_shape.min_size = heap_shape.min_size + 2
	    -- If min_size>1, this heap is in its final position if there isn't enough
	    -- room for the next Leonardo number and one extra element.
	    rectify = (len-last-2+init) < leonardo_numbers[heap_shape.min_size]+1
	  --Case 2: Otherwise, if the last heap has Leonardo number 1, add
	  --a singleton heap of Leonardo number 0.
	  elseif heap_shape.min_size==1 then
	    heap_shape.min_size = 0
	    -- If this last tree has order 0, then it's in its final position only
	    -- if it's the very last element of the array.
	    rectify = last+2-init==len
	  --Case 0: If there are no elements in the heap, add a tree of order 1.
	  --In all other cases, also add a singleton heap of Leonardo number 1.
	  else
	    heap_shape.min_size = 1
	    -- If this last tree has order 1, then it's in its final position if
	    -- it's the last element, or it's the penultimate element and it's not
	    -- about to be merged.  For simplicity
	    rectify = last+2-init==len or not heap_shape.flags[2]
	  end
	  heap_shape.flags[heap_shape.min_size] = true

	  if rectify then
	    rectify_lheap(lheap,heap_shape,init,last+1,heap_shape.min_size,comp)
	  else
	    rebalance_tree(lheap,last+1,heap_shape.min_size,comp)
	  end
	end

	-- Remove the rightmost element from the Leonardo heap and readjust if
	-- necessary.
	local function pop(lheap,heap_shape,init,last,comp)
	  -- Case 1: The last heap is of order zero or one.  In this case, removing it
	  -- doesn't expose any new trees and we can just drop it from the list of
	  -- trees, and find the next tree to set heap_shape.min_size.
	  if heap_shape.min_size<=1 then
	    heap_shape.flags[heap_shape.min_size] = false
	    repeat heap_shape.min_size = heap_shape.min_size + 1
	    until heap_shape.flags[heap_shape.min_size]
	    return
	  end
	  -- Case 2: The last heap is of order two or greater.  In this case, we
	  -- exposed two new heaps, which may require rebalancing.
	  heap_shape.flags[heap_shape.min_size] = false
	  local c1,c2 = child1(last,heap_shape.min_size),child2(last)
	  heap_shape.min_size = heap_shape.min_size-2
	  heap_shape.flags[heap_shape.min_size]=true
	  heap_shape.flags[heap_shape.min_size+1]=true
	  rectify_lheap(lheap,heap_shape,init,c1,heap_shape.min_size+1,comp)
	  rectify_lheap(lheap,heap_shape,init,c2,heap_shape.min_size,comp)
	end

	local smoothsort
	do
	  local lt = function(a,b) return a<b end
	  smoothsort = function(arr,len,comp)
	    len = len or #arr
	    comp = comp or lt
	    local init = 1
	    if len<=1 then return arr end
	    if arr[0] then init = 0 end
	    local heap_shape = heap_shape_t(n_leonardo_numbers,-1)
	    --Leonardo-heapify the array
	    for i=init,len+init-1 do
	      push(arr,heap_shape,init,i-1,len,comp)
	    end
	    --Pop the max element from the Leonardo heap until it's empty
	    for i=len+init-1,init,-1 do
	      pop(arr,heap_shape,init,i,comp)
	    end
	    return arr
	  end
	end

	return smoothsort
end

aaa.show_file_end( "sort" )