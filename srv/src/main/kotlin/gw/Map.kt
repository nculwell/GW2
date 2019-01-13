package gw

import java.util.Hashtable

val N_LAYERS = 2

data class MapRegion(val id: Int, val name: String) {
  val sectors = HashMap<Int, MapSector>()
}

val emptyCover = List<Cover>()
val emptyMovable = List<Movable>()

data class MapSector(val id: Int, val size: Size) {
  private val tiles = intArrayOf(size.w * size.h)
  private val cover = Hashtable<Point, List<Cover>>()
  private val movable = Hashtable<Point, List<Movable>>()
  // TODO: Encode connections between sectors somehow.
  fun tileAt(p: Point): Int {
    checkBounds(p, size)
    val rowOffset = size.w * p.y
    val colOffset = p.x
    val offset = rowOffset + colOffset
    val cell = tiles[offset]
    return cell
  }
  fun coverAt(p: Point): List<Cover> {
    checkBounds(p, size)
    return cover.get(p) ?: emptyCover
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

interface Movable

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
, Movable

data class ItemClass(val id: Int, val name: String, override val img: Image)
: Visible

data class Monster(val id: Int, val cls: MonsterClass)
: Locatable by Location(Point(-1, -1))
, Visible by cls
, Movable

data class MonsterClass(val id: Int, val name: String, override val img: Image)
: Visible

data class Player(val id: Int, val cls: PlayerClass)
: Locatable by Location(Point(-1, -1))
, Visible by cls
, Movable

data class PlayerClass(val id: Int, val name: String, override val img: Image)
: Visible

