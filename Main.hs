{-# LANGUAGE ForeignFunctionInterface #-}

module Main where

import Prelude
import Foreign.C
import Foreign.Ptr
import System.Environment

foreign import ccall rdOpcode :: CInt -> IO CInt
foreign import ccall rdConfigure :: CInt -> IO CInt
foreign import ccall rdTopologyMode :: CInt -> IO CInt
foreign import ccall rdFixedMode :: CInt -> IO CInt
foreign import ccall rdSubconf :: CInt -> IO CInt
foreign import ccall rdInt :: CInt -> IO CInt
foreign import ccall rdFloat :: CInt -> IO CFloat
foreign import ccall rdDouble :: CInt -> IO CDouble
foreign import ccall rdChars :: CInt -> CInt -> Ptr CChar -> IO ()
foreign import ccall rdInts :: CInt -> CInt -> Ptr CInt -> IO ()
foreign import ccall rdFloats :: CInt -> CInt -> Ptr CFloat -> IO ()
foreign import ccall rdDoubles :: CInt -> CInt -> Ptr CDouble -> IO ()
foreign import ccall wrOpcode :: CInt -> CInt -> IO ()
foreign import ccall wrConfigure :: CInt -> CInt -> IO ()
foreign import ccall wrTopologyMode :: CInt -> CInt -> IO ()
foreign import ccall wrFixedMode :: CInt -> CInt -> IO ()
foreign import ccall wrSubconf :: CInt -> CInt -> IO ()
foreign import ccall wrInt :: CInt -> CInt -> IO ()
foreign import ccall wrFloat :: CInt -> CFloat -> IO ()
foreign import ccall wrDouble :: CInt -> CDouble -> IO ()
foreign import ccall wrChars :: CInt -> CInt -> Ptr CChar -> IO ()
foreign import ccall wrInts :: CInt -> CInt -> Ptr CInt -> IO ()
foreign import ccall wrFloats :: CInt -> CInt -> Ptr CFloat -> IO ()
foreign import ccall wrDoubles :: CInt -> CInt -> Ptr CDouble -> IO ()
foreign import ccall exOpcode :: CInt -> CInt -> IO ()
foreign import ccall hello :: CString -> IO CString

main = do
   args <- getArgs
   print (map (\x -> ((read x)::Int)) args)
   (newCString "hello") >>= hello >>= peekCString >>= print
