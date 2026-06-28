module {
  // TEST 1: Isme factor 1.0 hai, toh hamara pass isko hata kar direct input return karega!
  func.func @test_brighten_optimize(%input: tensor<256x256xf32>) -> tensor<256x256xf32> {
    %0 = "image.brighten"(%input) {factor = 1.0 : f32} : (tensor<256x256xf32>) -> tensor<256x256xf32>
    return %0 : tensor<256x256xf32>
  }

  // TEST 2: Isme factor 2.5 hai, toh ye operation as-is rehna chahiye, remove nahi hoga.
  func.func @test_brighten_no_optimize(%input: tensor<256x256xf32>) -> tensor<256x256xf32> {
    %0 = "image.brighten"(%input) {factor = 2.5 : f32} : (tensor<256x256xf32>) -> tensor<256x256xf32>
    return %0 : tensor<256x256xf32>
  }
}
