package gw

public class ContentTree {
  var top = Empty()
//  public fun addItem(item: Item, pt: Point) {
//    val node = findNode(top, pt)
//    // TODO: Add
//  }
}

sealed class Content
class Empty: Content()
data class ContentItem(val item: Item) : Content()
data class ContentPlayer(val player: Player) : Content()
data class ContentMonster(val monster: Monster) : Content()
data class Node(val parent: Node, val rect: Rect) : Content() {
  var left = Empty()
  var right = Empty()
}

//fun findNode(top: Content, loc: Point): Node {
  // TODO: Figure out which half the point is in, then
  // check the appropriate side.
//}

