
public var fountainPrefab : GameObject ; 
private var _transform : Transform ; 


function Start() 
{
	_transform = GetComponent ( Transform ) as Transform ; 	
}

function OnTriggerEnter (other : Collider) {
  
      
    // Play a sound if the coliding objects had a big impact.        
 //   if (collision.relativeVelocity.magnitude > 5 )
   // {
    	var _go : GameObject = Instantiate ( fountainPrefab , _transform.position, Quaternion.identity ) as GameObject ; 	
		yield WaitForSeconds( 5.0 ) ; 
		var _emitter : ParticleEmitter = _go.GetComponent ( ParticleEmitter ) as ParticleEmitter ; 
		_emitter.emit = false ;  
		
    //}
}

function Update () {
}