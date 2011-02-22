//@script RequireComponent( Rigidbody ) 

public var triggerForce : float = 4.0f; 
private var _rigidBody : Rigidbody ; 


function Start() 
{ 
	_rigidBody = GetComponent ( Rigidbody ) as Rigidbody ; 
	//_gameObject = GetComponent( GameObject ) as GameObject ; 
}

function OnCollisionEnter (collision : Collision) 
{
	if ( collision.gameObject.tag != "BuildingSupport" ) 
	{
		if (collision.relativeVelocity.magnitude > triggerForce )
		{
			_rigidBody.useGravity = true ; 
			yield WaitForSeconds(5);
			Destroy( _rigidBody ) ; 
			Destroy( GetComponent ( BoxCollider ) as BoxCollider ) ; 
		}
		else
		{
			_rigidBody.velocity = Vector3.zero ; 
			_rigidBody.angularVelocity = Vector3.zero ; 
		}
	}
   
	/*   
    if ( _gameObject ) 
    	_gameObject.AddComponent(Rigidbody) ;
    	*/
}

function Update () {
}