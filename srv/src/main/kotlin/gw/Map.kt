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

fun checkBounds(p: Point, size: Size) {
  if (p.x < 0) { throw Exception("x < 0") }
  if (p.y < 0) { throw Exception("y < 0") }
  if (p.y >= size.h) { throw Exception("y > height") }
  if (p.y >= size.w) { throw Exception("x > width") }
}

data class MapSector(val id: Int, val size: Size) {
  val tiles = intArrayOf(size.w * size.h)
  val cover = Hashtable<Point, Cover>()
  // TODO: Encode connections between sectors somehow.
  fun tileAt(p: Point): Int {
    checkBounds(p, size)
    val rowOffset = size.w * p.y
    val colOffset = p.x
    val offset = rowOffset + colOffset
    val cell = tiles[offset]
    return cell
  }
}

// This should match the storage format for the map.
// Flatten into a larger number of composite tiles to give
// a single-layer map that's easier to work with.
data class MapSectorLayered(val id: Int, val size: Size, val nLayers: Int) {
  val tiles = intArrayOf(nLayers * size.w * size.h)
  val cover = Hashtable<Point, Cover>()
  // TODO: Encode connections between sectors somehow.
  fun tilesAt(p: Point): List<Int> {
    checkBounds(p, size)
    val rowOffset = nLayers * size.w * p.y
    val colOffset = nLayers * p.x
    val offset = rowOffset + colOffset
    val cell = tiles.slice(offset..(offset+nLayers-1))
    return cell
  }
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

