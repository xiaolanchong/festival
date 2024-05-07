

SET(hdr_files
	advanced_deadlock.h
	advisors.h
	bfs.h
	biconnect.h
	board.h
	corral.h
	corral_deadlock.h
	cycle_deadlock.h
	deadlock.h
	deadlock_cache.h
	deadlock_utils.h
	debug.h
	diags.h
	distance.h
	dragonfly.h
	dragonfly_queue.h
	engine.h
	envelope.h
	fixed_boxes.h
	girl.h
	global.h
	graph.h
	greedy.h
	helper.h
	heuristics.h
	hf_search.h
	holes.h
	hotspot.h
	hungarian.h
	imagine.h
	interseting.h
	io.h
	k_dist_deadlock.h
	match_distance.h
	max_dist.h
	mini_search.h
	moves.h
	mpdb2.h
	naive.h
	oop.h
	opener.h
	order.h
	overlap.h
	packing_search.h
	park_order.h
	perimeter.h
	positions.h
	preprocess.h
	queue.h
	request.h
	rooms.h
	rooms_deadlock.h
	score.h
	snail.h
	sol.h
	stuck.h
	textfile.h
	tree.h
	util.h
	wobblers.h
	xy_deadlock.h
)

SET(src_files
	advanced_deadlock.cpp
	advisors.cpp
	bfs.cpp
	biconnect.cpp
	board.cpp
	corral.cpp
	corral_deadlock.cpp
	cycle_deadlock.cpp
	deadlock.cpp
	deadlock_cache.cpp
	deadlock_utils.cpp
	debug.cpp
	diags.cpp
	distance.cpp
	dragonfly.cpp
	dragonfly_queue.cpp
	engine.cpp
	envelope.cpp
	fixed_boxes.cpp
	girl.cpp
	global.cpp
	graph.cpp
	greedy.cpp
	helper.cpp
	heuristics.cpp
	hf_search.cpp
	holes.cpp
	hotspot.cpp
	hungarian.cpp
	imagine.cpp
	io.cpp
	k_dist_deadlock.cpp
	lurd.cpp
	match_distance.cpp
	max_dist.cpp
	mini_search.cpp
	moves.cpp
	mpdb2.cpp
	naive.cpp
	oop.cpp
	opener.cpp
	order.cpp
	overlap.cpp
	packing_search.cpp
	park_order.cpp
	perimeter.cpp
	positions.cpp
	preprocess.cpp
	queue.cpp
	request.cpp
	rooms.cpp
	rooms_deadlock.cpp
	score.cpp
	snail.cpp
	sokoban_solver.cpp
	sol.cpp
	stuck.cpp
	textfile.cpp
	tree.cpp
	util.cpp
	wobblers.cpp
	xy_deadlock.cpp
)

add_executable(festival ${hdr_files} ${src_files})

if (MSVC)
    # Force to compile with W4
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		add_compile_options(/W4)
	endif()
	set_property(TARGET festival PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL<$<CONFIG:Debug>:Debug>")
    set_property(TARGET festival PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreadedDLL<$<CONFIG:Release>:Release>")
	target_link_options(festival PRIVATE /DEBUG)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
	add_compile_definitions(LINUX)
	add_compile_definitions(THREADS)
    # additional warnings
    add_compile_options(-Wall -Wextra -Wpedantic)
	target_link_libraries(festival pthread)
endif()
