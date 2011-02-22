#pragma strict

public var _windowPane : GameObject ; 

public var rows : int = 8 ; 
public var columns : int = 8 ; 
public var numberOfLevels : int = 10 ; 
public var ySpacing : float = 4.0 ; 

private var _transform : Transform ; 

function Start() 
{
	_transform = GetComponent ( Transform ) as Transform ; 
	var pieces : Array = new Array() ; 
		
	for ( var _y : int = 0 ; _y < columns ; _y++ ) 
	{
		for ( var _x : int = 0 ; _x < rows ; _x++ ) 
		{
			var go : GameObject = Instantiate( _windowPane , Vector3(1,1,1) , Quaternion.identity ) as GameObject ;
			go.transform.position = Vector3( _transform.position.x + _x * 1.01 , _transform.position.y + _y * 1.01f  , _transform.position.z ) ; 
			go.transform.Rotate( Vector3( 0 , 90 , 90 ) ) ; 
			pieces[ _x + _y * rows ] = go ; 
			
		}
	}				
	
}

function Update () 
{
		
}