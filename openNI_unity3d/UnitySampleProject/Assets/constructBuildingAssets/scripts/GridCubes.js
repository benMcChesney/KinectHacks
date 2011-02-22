#pragma strict

public var _floorPiece : GameObject ; 
public var _columnPrefab : GameObject ; 
public var _windowPane : GameObject ; 

public var rows : int = 8 ; 
public var columns : int = 8 ; 
public var numberOfLevels : int = 10 ; 
public var ySpacing : float = 4.0 ; 

private var _transform : Transform ; 

function Start() 
{
	_transform = GetComponent( Transform ) as Transform ; 
	
	var offset : float = -.75 ; 
	var multiplier : float = 1.04f ; 
	var localYScale : float = numberOfLevels * ySpacing ; 
	var localYPos : float = _transform.position.y + (numberOfLevels * ySpacing) / 2 ; 
	var column1 : GameObject = Instantiate( _columnPrefab , Vector3( _transform.position.x + offset,
																	 localYPos ,
																	 _transform.position.z + offset) , Quaternion.identity ) as GameObject ;
	column1.transform.localScale = Vector3( 0.5f, localYScale , 0.5f ) ; 														 
	
	var column2 : GameObject = Instantiate( _columnPrefab , Vector3( _transform.position.x + rows*multiplier,
																	 localYPos,
																	 _transform.position.z + offset) , Quaternion.identity ) as GameObject ;
	column2.transform.localScale = Vector3( 0.5f, localYScale , 0.5f ) ; 							
										 
	var column3 : GameObject = Instantiate( _columnPrefab , Vector3( _transform.position.x + rows*multiplier ,
																	 localYPos,
																	 _transform.position.z + columns*multiplier) , Quaternion.identity ) as GameObject ;
	column3.transform.localScale = Vector3( 0.5f, localYScale , 0.5f ) ; 
	
	var column4 : GameObject = Instantiate( _columnPrefab , Vector3( _transform.position.x + offset ,
																	 localYPos,
																	 _transform.position.z + columns*multiplier) , Quaternion.identity ) as GameObject ;
	column4.transform.localScale = Vector3( 0.5f, localYScale , 0.5f ) ; 
	
	column1.tag = "BuildingSupport" ; 
	column2.tag = "BuildingSupport" ; 
	column3.tag = "BuildingSupport" ; 
	column4.tag = "BuildingSupport" ; 
		
	var fixedJoint : FixedJoint ;
	var _breakForce : float = 150.0f ; 
	var _torqueBreakForce : float = 150.0f ; 
		
	//Column1
	fixedJoint = column1.AddComponent( FixedJoint ) as FixedJoint ; 
	fixedJoint.breakForce = _breakForce ; 
	fixedJoint.breakTorque = _torqueBreakForce ; 
	fixedJoint.connectedBody = column2.GetComponent( Rigidbody ) as Rigidbody ; 
	fixedJoint = column1.AddComponent( FixedJoint ) as FixedJoint ; 
	fixedJoint.breakForce = _breakForce ; 
	fixedJoint.breakTorque = _torqueBreakForce ; 
	fixedJoint.connectedBody = column4.GetComponent( Rigidbody ) as Rigidbody ;
	
	//Column2
	fixedJoint = column2.AddComponent( FixedJoint ) as FixedJoint ; 
	fixedJoint.breakForce = _breakForce ; 
	fixedJoint.breakTorque = _torqueBreakForce ; 
	fixedJoint.connectedBody = column1.GetComponent( Rigidbody ) as Rigidbody ;
	fixedJoint = column2.AddComponent( FixedJoint ) as FixedJoint ; 
	fixedJoint.breakForce = _breakForce ; 
	fixedJoint.breakTorque = _torqueBreakForce ; 
	fixedJoint.connectedBody = column3.GetComponent( Rigidbody ) as Rigidbody ;
	
	//Column3
	fixedJoint = column3.AddComponent( FixedJoint ) as FixedJoint ; 
	fixedJoint.breakForce = _breakForce ; 
	fixedJoint.breakTorque = _torqueBreakForce ; 
	fixedJoint.connectedBody = column2.GetComponent( Rigidbody ) as Rigidbody ;
	fixedJoint = column3.AddComponent( FixedJoint ) as FixedJoint ; 
	fixedJoint.breakForce = _breakForce ; 
	fixedJoint.breakTorque = _torqueBreakForce ; 
	fixedJoint.connectedBody = column4.GetComponent( Rigidbody ) as Rigidbody ;
	
	//Column4
	fixedJoint = column4.AddComponent( FixedJoint ) as FixedJoint ; 
	fixedJoint.breakForce = _breakForce ; 
	fixedJoint.breakTorque = _torqueBreakForce ; 
	fixedJoint.connectedBody = column1.GetComponent( Rigidbody ) as Rigidbody ;
	fixedJoint = column4.AddComponent( FixedJoint ) as FixedJoint ; 
	fixedJoint.breakForce = _breakForce ; 
	fixedJoint.breakTorque = _torqueBreakForce ; 
	fixedJoint.connectedBody = column3.GetComponent( Rigidbody ) as Rigidbody ;
		
	var sideMultipliers : Array = new Array() ;
	sideMultipliers.push( Vector3( 1 , 0 , 1 ) ); 
	sideMultipliers.Push( Vector3( 0 , 1 , 1 ) ); 
	sideMultipliers.Push( Vector3( 1 , 0 , 1 ) ); 
	sideMultipliers.Push( Vector3( 0 , 1 , 0 ) ); 
		
	for ( var p : int = 1 ; p < numberOfLevels+1 ; p++ ) 
	{
		var pieces : Array = new Array() ; 
		
		for ( var _y : int = 0 ; _y < columns ; _y++ ) 
		{
			for ( var _x : int = 0 ; _x < rows ; _x++ ) 
			{
				var go : GameObject = Instantiate( _floorPiece , Vector3(1,1,1) , Quaternion.identity ) as GameObject ;
				go.transform.position = Vector3( _transform.position.x + _x * 1.01 ,
												 _transform.position.y + ySpacing * p ,
												 _transform.position.z + _y * 1.01f ) ; 
				pieces[ _x + _y * rows ] = go ; 
			}
		}		
		yield WaitForSeconds( 0.1f ) ; 		
	}
	
		/*
	for ( var side : int = 0 ; side < 4 ; side++ )
	{
		for ( var _c : int = 0 ; _c < columns ; _c++ ) 
		{
			for ( var _r : int = 0 ; _r < rows ; _r++ ) 
			{
				var _sMultiplier = sideMultipliers[side] ; 
				var _go : GameObject = Instantiate( _windowPane , Vector3(1,1,1) , Quaternion.identity ) as GameObject ;
				_go.transform.position = Vector3( _transform.position.x + _r * 1.01f * _sMultiplier.x ,
												  _transform.position.y + _c * 1.01f * _sMultiplier.y ,
												  _transform.position.z * _sMultiplier.z ) ; 
				_go.transform.Rotate( Vector3( 0 , 90 * side , 90 ) ) ; 
			}
		}
	}		
	*/	
		/*
		for ( var __y : int = 0 ; __y < columns ; __y++ ) 
		{
			for ( var __x : int = 0 ; __x < rows ; __x++ ) 
			{

				var index : int = __x + __y * rows ;
			
			 	
				var prevIndex : int = index -1 ; 
				var nextIndex : int = index + 1 ; 
				var nextRowIndex : int = __x + ( __y - 1 ) * rows ; 
				var prevRowIndex : int = __x + ( __y + 1 ) * rows ; 
				var totalItems : int = rows * columns ; 
				//Debug.Log ( "index " + index + " < " + (rows * columns - 2) ) ; 
					
				if ( index < ( rows * columns )  ) 
				{
					var _go : GameObject = pieces[ index ] as GameObject; 
					_go.name = "floor_"+p.ToString()+"_piece"+index.ToString() ; 
					
					if ( index == 0 ) 
					{
						_go.name = "floor_"+p.ToString()+"corner01" ;
					}
					if ( index == rows +- 1 ) 
					{
						_go.name = "floor_"+p.ToString()+"corner02" ;					
					}
					if ( index == totalItems-1 ) 
					{
						_go.name = "floor_"+p.ToString()+"corner03" ;					
					}
					if ( index == totalItems - columns )
					{
						_go.name = "floor_"+p.ToString()+"corner04" ;					
					}
					/*
					var fixedJoint : FixedJoint ;
					var _breakForce : float = 20.0f ; 
					var _torqueBreakForce : float = 20.0f ; 
						
					if ( prevIndex > 0 && prevIndex < totalItems && __x != rows-1 ) 
					{
						fixedJoint = _go.AddComponent( FixedJoint ) as FixedJoint ; 
						fixedJoint.breakForce = _breakForce ; 
						fixedJoint.breakTorque = _torqueBreakForce ; 
						fixedJoint.connectedBody = (pieces[ prevIndex ] as GameObject).GetComponent( Rigidbody ) as Rigidbody ; 
					}			
			
				}
			}
		}*/
}

function Update () 
{
		
}