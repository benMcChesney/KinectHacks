public var _sparksParticles : ParticleEmitter ; 
public var _light : Light ; 


function OnTriggerEnter (other : Collider) {

    // Play a sound if the coliding objects had a big impact.        
 //   if (collision.relativeVelocity.magnitude > 5.0f)
//	{
		_light.intensity = 0.01f ; 
		_sparksParticles.emit = true ; 	
//	}
}
function Update () {
}