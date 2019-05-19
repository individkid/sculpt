{-# LANGUAGE ForeignFunctionInterface #-}

module Main where

import Prelude
import Foreign.C

foreign import ccall hello :: CString -> IO CString

main = do
   (newCString "hello") >>= hello >>= peekCString >>= print
