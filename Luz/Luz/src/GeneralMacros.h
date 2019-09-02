#pragma once
#ifndef GENERALMACROS_H
#define GENERALMACROS_H

#define DEFAULT_COPY_CONSTRUCTOR(Class) Class(const Class&) = default;
#define DEFAULT_COPY_ASSIGNMENT(Class) Class& operator=(const Class&) = default;

#define DEFAULT_COPY(Class)\
DEFAULT_COPY_CONSTRUCTOR(Class)\
DEFAULT_COPY_ASSIGNMENT(Class)

#define DEFAULT_MOVE_CONSTRUCTOR(Class) Class(Class&&) = default;
#define DEFAULT_MOVE_ASSIGNMENT(Class) Class& operator=(Class&&) = default;

#define DEFAULT_MOVE(Class)\
DEFAULT_MOVE_CONSTRUCTOR(Class)\
DEFAULT_MOVE_ASSIGNMENT(Class)


#define NO_COPY_CONSTRUCTOR(Class) private: Class(const Class&) = delete;
#define NO_COPY_ASSIGNMENT(Class) private: Class& operator=(const Class&) = delete;

#define NO_COPY(Class)\
NO_COPY_CONSTRUCTOR(Class)\
NO_COPY_ASSIGNMENT(Class)

#define NO_MOVE_CONSTRUCTOR(Class) private: Class(Class&&) = delete;
#define NO_MOVE_ASSIGNMENT(Class) private: Class& operator=(Class&&) = delete;

#define NO_MOVE(Class)\
NO_MOVE_CONSTRUCTOR(Class)\
NO_MOVE_ASSIGNMENT(Class)

#endif
