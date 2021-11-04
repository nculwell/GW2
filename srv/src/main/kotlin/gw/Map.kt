package gw

import java.util.Hashtable

data class MapRegion(val id: Int, val name: String) {
  val sectors = HashMap<Int, MapSector>()
}

val emptyCover: List<Cover> = emptyList()
val emptyMovable: List<Movable> = emptyList()

data class MapSector(val id: Int, val size: Size) {
  private val tiles = IntArray(size.w * size.h)
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
  private fun movableAt(p: Point): List<Movable> {
    checkBounds(p, size)
    return movable.get(p) ?: emptyMovable
  }
  fun itemsAt(p: Point): List<Item> {
    val ms = movableAt(p)
    val items = ms.map({ i -> i as Item })
    return items.filterNotNull()
  }
  fun getView(r: Rect): MapView {
  }
}

data class MapView(
    val tiles: IntArray,
    val cover: Array<List<Cover>>,
    val items: Array<List<Item>>,
    val beings: Array<Being>
)

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

interface Movable

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

interface Animate
{
  val id: Int
  var hpMax: Int
  var hp: Int
}

data class Being(override val id: Int, override var hpMax: Int)
: Animate
, Locatable by Location(Point(-1, -1))
, Movable
{
  override var hp: Int = hpMax
}

data class BeingClass(val id: Int, val name: String, override val img: Image)
: Visible

data class Monster(val cls: MonsterClass, val being: Being)
: Animate by being
, Visible by cls

data class MonsterClass(val id: Int, val name: String, override val img: Image)
: Visible

class Player(id: Int, cls: PlayerClass)
: Animate by being
, Visible by cls
{
  val being = Being(id, 0)
  val cls = cls
}

data class PlayerClass(val id: Int, val name: String, override val img: Image)
: Visible

