# Yes Synonyms
# task, work, job, duty, chore, charge, assignment, detail, mission,
# engagement, occupation, undertaking, exercise, business, responsibility,
# burden, endeavor, enterprise, venture


SET(TASK_RUNNER_SRC
    Task.h
    main.cpp)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(task_main ${TASK_RUNNER_SRC})
# TARGET_LINK_LIBRARIES(task_main )
SET_PROPERTY(TARGET task_main PROPERTY CXX_STANDARD 14)
