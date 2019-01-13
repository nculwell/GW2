package gw

import java.util.Hashtable

data class Point(val x: Int, val y: Int) {
  operator fun plus(q: Point) = Point(x+q.x, y+q.y)
  operator fun plus(q: Size) = Point(x+q.w, y+q.h)
  operator fun minus(q: Point) = Point(x-q.x, y-q.y)
  operator fun minus(q: Size) = Point(x-q.w, y-q.h)
  operator fun times(q: Size) = Point(x*q.w, y*q.h)
  operator fun div(q: Size) = Point(x/q.w, y/q.h)
}

data class Size(val w: Int, val h: Int) {
  operator fun plus(q: Size) = Size(w+q.w, h+q.h)
  operator fun minus(q: Size) = Size(w-q.w, h-q.h)
  operator fun times(q: Size) = Size(w*q.w, h*q.h)
  operator fun div(q: Size) = Size(w/q.w, h/q.h)
}

data class Rect(val pt: Point, val sz: Size) {
  constructor(x: Int, y: Int, w: Int, h: Int)
  : this(Point(x, y), Size(w, h))
  {}
}

fun checkBounds(p: Point, size: Size) {
  if (p.x < 0) { throw Exception("x < 0") }
  if (p.y < 0) { throw Exception("y < 0") }
  if (p.x >= size.w) { throw Exception("x > width") }
  if (p.y >= size.h) { throw Exception("y > height") }
}

