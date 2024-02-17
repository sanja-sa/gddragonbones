@tool
extends EditorScript

func _run() -> void:
	var res = ResourceLoader.load("res://assets/dragonbones_assets/AncientAutomaton_ske.json", "GDDragonBonesResource")
	print(res, " = ")
	#print("res://icon.svg".get_basename())

