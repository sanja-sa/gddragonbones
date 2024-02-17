@tool
extends GDDragonBones


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	for c:CanvasItem in get_child(0).get_children():
		#print(c.texture)
		c.queue_redraw()
	queue_redraw()
