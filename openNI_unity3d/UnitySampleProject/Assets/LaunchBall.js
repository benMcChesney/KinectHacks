private var _rb : Rigidbody ; 
public var _force : Vector3 = Vector3.zero ; 
function Start() 
{
	_rb = GetComponent ( Rigidbody ) as Rigidbody ; 
	_rb.AddForce ( _force ) ; 
}