include "mlir/IR/OpBase.td"

def Image_Dialect:Dialect{
	let name="image";
	let cppNamespace="::image";
	let description=[{
		The Image processing dialect.
	}];
}
