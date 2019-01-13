package gw

import java.util.Hashtable

val N_LAYERS = 2

data class Point(val x: Int, val y: Int) {
  operator fun plus(q: Point) = Point(x+q.x, y+q.y)
  operator fun minus(q: Point) = Point(x-q.x, y-q.y)
  operator fun times(q: Size) = Point(x*q.w, y*q.h)
  operator fun div(q: Size) = Point(x/q.w, y/q.h)
}

data class Size(val w: Int, val h: Int) {
  operator fun plus(q: Size) = Size(w+q.w, h+q.h)
  operator fun minus(q: Size) = Size(w-q.w, h-q.h)
  operator fun div(q: Size) = Size(w/q.w, h/q.h)
}

data class Rect(val pt: Point, val sz: Size) {
  constructor(x: Int, y: Int, w: Int, h: Int)
  : this(Point(x, y), Size(w, h))
  {}
}

data class MapRegion(val id: Int, val name: String) {
  val sectors = HashMap<Int, MapSector>()
}

class MapSector(id: Int, size: Size) {
  val id = id
  val tiles = intArrayOf(N_LAYERS * size.w * size.h)
  val cover = Hashtable<Point, Cover>()
  // TODO: Encode connections between sectors somehow.
}

data class Image(val id: Int, val filename: String, val quad: Rect)

interface Visible {
  val img: Image
}

interface Locatable {
  var loc: Point
}

data class Location(override var loc: Point)
: Locatable

data class Cover(val id: Int, val cls: CoverClass)
: Locatable by Location(Point(-1, -1))
, Visible by cls

data class CoverClass(val id: Int, val name: String, override val img: Image)
: Visible

data class Item(val id: Int, val cls: ItemClass)
: Locatable by Location(Point(-1, -1))
, Visible by cls

data class ItemClass(val id: Int, val name: String, override val img: Image)
: Visible

data class Monster(val id: Int, val cls: MonsterClass)
: Locatable by Location(Point(-1, -1))
, Visible by cls

data class MonsterClass(val id: Int, val name: String, override val img: Image)
: Visible

data class Player(val id: Int, val cls: PlayerClass)
: Locatable by Location(Point(-1, -1))
, Visible by cls

data class PlayerClass(val id: Int, val name: String, override val img: Image)
: Visible

