--    Main interface through C to another process
--    Copyright (C) 2016  Paul Coelho
--
--    This program is free software: you can redistribute it and/or modify
--    it under the terms of the GNU General Public License as published by
--    the Free Software Foundation, either version 3 of the License, or
--    (at your option) any later version.
--
--    This program is distributed in the hope that it will be useful,
--    but WITHOUT ANY WARRANTY; without even the implied warranty of
--    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--    GNU General Public License for more details.
--
--    You should have received a copy of the GNU General Public License
--    along with this program.  If not, see <http://www.gnu.org/licenses/>.

{-# LANGUAGE ForeignFunctionInterface #-}

module Main where

import Foreign.C
import Foreign.Ptr
import System.Environment
import Data.Vector hiding (map,replicate,length)
import AffTopo.Naive hiding (Vector)

data State = State {
   macroData :: Vector (Vector (Ptr ())),
   macroScript :: Vector (Vector (Ptr ())),
   hotkeyData :: Vector (Ptr ()),
   hotkeyScript :: Vector (Ptr ())
}

setMacroData :: State -> Vector (Vector (Ptr ())) -> State
setMacroData a b = a {macroData = b}

setMacroScript :: State -> Vector (Vector (Ptr ())) -> State
setMacroScript a b = a {macroScript = b}

setHotkeyData :: State -> Vector (Ptr ()) -> State
setHotkeyData a b = a {hotkeyData = b}

setHotkeyScript :: State -> Vector (Ptr ()) -> State
setHotkeyScript a b = a {hotkeyScript = b}

set2 :: CInt -> CInt -> t -> Vector (Vector t) -> Vector (Vector t)
set2 a b c v = v // [((fromIntegral a), (v ! (fromIntegral a)) // [((fromIntegral b), c)])]

set1 :: CInt -> t -> Vector t -> Vector t
set1 a b v = v // [((fromIntegral a), b)]

rmw2 :: (State -> Vector (Vector t)) -> (State -> Vector (Vector t) -> State) -> CInt -> CInt -> t -> IO State -> IO State
rmw2 f g a b c u = fmap (\x -> (g x (set2 a b c (f x)))) u

rmw1 :: (State -> Vector t) -> (State -> Vector t -> State) -> CInt -> t -> IO State -> IO State
rmw1 f g a b u = fmap (\x -> (g x (set1 a b (f x)))) u

rdInts :: CInt -> CInt -> IO [CInt]
rdInts rdfd count
   | count == 0 = return []
   | count > 0 = do
   int <- rdInt rdfd
   ints <- rdInts rdfd (count - 1)
   return (int:ints)
   | otherwise = undefined

wrInts :: CInt -> [CInt] -> IO ()
wrInts wrfd [] = return ()
wrInts wrfd (a:b) = do
   wrInt wrfd a
   wrInts wrfd b

toInt :: Integral a => a -> Int
toInt = fromInteger . toInteger

toCInt :: Integral a => Int -> a
toCInt = fromInteger . toInteger

emptyState :: IO State
emptyState = undefined

mainLoop :: CInt -> CInt -> Enumeration -> IO State -> IO State
mainLoop rdfd wrfd en state = do
   src <- rdOpcode rdfd
   ptr <- rdPointer rdfd
   exOpcode rdfd (fileOp en)
   file <- rdInt rdfd
   exOpcode rdfd (planeOp en)
   plane <- rdInt rdfd
   exOpcode rdfd (confOp en)
   conf <- rdConfigure rdfd
   mainLoop rdfd wrfd en (mainIter rdfd wrfd en src ptr file plane conf state)

mainIter :: CInt -> CInt -> Enumeration -> CInt -> Ptr () -> CInt -> CInt -> CInt -> IO State -> IO State
mainIter rdfd wrfd en src ptr file plane conf state
   | src == (readOp en) = readIter rdfd wrfd en ptr file plane conf state
   | src == (windowOp en) = windowIter rdfd wrfd en ptr file plane conf state
   | otherwise = undefined

readIter :: CInt -> CInt -> Enumeration -> Ptr () -> CInt -> CInt -> CInt -> IO State -> IO State
readIter rdfd wrfd en ptr file plane conf state
   | conf == (onceConf en) = do
   exOpcode rdfd (funcOp en)
   func <- rdFunction rdfd
   exOpcode rdfd (countOp en)
   count <- rdInt rdfd
   exOpcode rdfd (specifyOp en)
   specify <- rdInts rdfd count
   exOpcode rdfd (scriptOp en)
   script <- rdPointer rdfd
   wrOpcode wrfd (readOp en)
   wrPointer wrfd ptr
   queryIter wrfd en func count specify script state
   | conf == (macroConf en) = undefined
   | conf == (hotkeyConf en) = undefined
   | otherwise = undefined

windowIter :: CInt -> CInt -> Enumeration -> Ptr () -> CInt -> CInt -> CInt -> IO State -> IO State
windowIter rdfd wrfd en file plane conf state
   | conf == (clickConf en) = undefined
   -- TODO send Query based on script saved under file and plane
   | conf == (pressConf en) = undefined
   -- TODO send Query based on script saved under press
   | otherwise = undefined

queryIter :: CInt -> Enumeration -> CInt -> CInt -> [CInt] -> Ptr () -> IO State -> IO State
queryIter wrfd en func count specify script state
   | func == (attachedFunc en) = do
   wrOpcode wrfd (scriptOp en)
   wrPointer wrfd script
   ints <- attachedIter specify state
   wrOpcode wrfd (givenOp en)
   wrGiven wrfd (intsGiv en)
   wrOpcode wrfd (lengthOp en)
   wrInt wrfd (toCInt (length ints))
   wrOpcode wrfd (intsOp en)
   wrInts wrfd ints
   state

macroIter :: Ptr () -> CInt -> CInt -> Ptr () -> IO State -> IO State
macroIter ptr file plane script state = let
   state1 = rmw2 macroData setMacroData file plane ptr state
   in rmw2 macroScript setMacroScript file plane script state1

hotkeyIter :: Ptr () -> CInt -> Ptr () -> IO State -> IO State
hotkeyIter ptr key script state = let
   state1 = rmw1 hotkeyData setHotkeyData key ptr state
   in rmw1 hotkeyScript setHotkeyScript key script state1

attachedIter :: [CInt] -> IO State -> IO [CInt]
attachedIter specify state = undefined

foreign import ccall setDebug :: CInt -> IO ()
foreign import ccall enumerate :: Ptr CChar -> IO CInt

foreign import ccall rdPointer :: CInt -> IO (Ptr ())
foreign import ccall rdOpcode :: CInt -> IO CInt
foreign import ccall rdSource :: CInt -> IO CInt
foreign import ccall rdConfigure :: CInt -> IO CInt
foreign import ccall rdEvent :: CInt -> IO CInt
foreign import ccall rdChange :: CInt -> IO CInt
foreign import ccall rdGiven :: CInt -> IO CInt
foreign import ccall rdSubconf :: CInt -> IO CInt
foreign import ccall rdSculpt :: CInt -> IO CInt
foreign import ccall rdClickMode :: CInt -> IO CInt
foreign import ccall rdMouseMode :: CInt -> IO CInt
foreign import ccall rdRollerMode :: CInt -> IO CInt
foreign import ccall rdTargetMode :: CInt -> IO CInt
foreign import ccall rdTopologyMode :: CInt -> IO CInt
foreign import ccall rdFixedMode :: CInt -> IO CInt
foreign import ccall rdField :: CInt -> IO CInt
foreign import ccall rdBuffer :: CInt -> IO CInt
foreign import ccall rdProgram :: CInt -> IO CInt
foreign import ccall rdFunction :: CInt -> IO CInt
foreign import ccall rdEquate :: CInt -> IO CInt
foreign import ccall rdFactor :: CInt -> IO CInt
foreign import ccall rdChar :: CInt -> IO CChar
foreign import ccall rdInt :: CInt -> IO CInt
foreign import ccall rdFloat :: CInt -> IO CFloat
foreign import ccall rdDouble :: CInt -> IO CDouble

foreign import ccall wrPointer :: CInt -> Ptr () -> IO ()
foreign import ccall wrOpcode :: CInt -> CInt -> IO ()
foreign import ccall wrSource :: CInt -> CInt -> IO ()
foreign import ccall wrConfigure :: CInt -> CInt -> IO ()
foreign import ccall wrEvent :: CInt -> CInt -> IO ()
foreign import ccall wrChange :: CInt -> CInt -> IO ()
foreign import ccall wrGiven :: CInt -> CInt -> IO ()
foreign import ccall wrSubconf :: CInt -> CInt -> IO ()
foreign import ccall wrSculpt :: CInt -> CInt -> IO CInt
foreign import ccall wrClickMode :: CInt -> CInt -> IO CInt
foreign import ccall wrMouseMode :: CInt -> CInt -> IO CInt
foreign import ccall wrRollerMode :: CInt -> CInt -> IO CInt
foreign import ccall wrTargetMode :: CInt -> CInt -> IO CInt
foreign import ccall wrTopologyMode :: CInt -> CInt -> IO ()
foreign import ccall wrFixedMode :: CInt -> CInt -> IO ()
foreign import ccall wrField :: CInt -> CInt -> IO ()
foreign import ccall wrBuffer :: CInt -> CInt -> IO ()
foreign import ccall wrProgram :: CInt -> CInt -> IO ()
foreign import ccall wrFunction :: CInt -> CInt -> IO ()
foreign import ccall wrEquate :: CInt -> CInt -> IO ()
foreign import ccall wrFactor :: CInt -> CInt -> IO ()
foreign import ccall wrChar :: CInt -> CChar -> IO ()
foreign import ccall wrInt :: CInt -> CInt -> IO ()
foreign import ccall wrFloat :: CInt -> CFloat -> IO ()
foreign import ccall wrDouble :: CInt -> CDouble -> IO ()

foreign import ccall exOpcode :: CInt -> CInt -> IO ()

data Enumeration = Enumeration {
   -- Sideband
   -- TODO for new Query pointer
   -- Message
   readOp :: CInt,
   writeOp :: CInt,
   scriptOp :: CInt,
   windowOp :: CInt,
   commandOp :: CInt,
   -- Command
   fileOp :: CInt,
   sourceOp :: CInt,
   subconfOp :: CInt,
   settingOp :: CInt,
   sculptOp :: CInt,
   clickOp :: CInt,
   mouseOp :: CInt,
   rollerOp :: CInt,
   targetOp :: CInt,
   topologyOp :: CInt,
   fixedOp :: CInt,
   matrixOp :: CInt,
   feedbackOp :: CInt,
   finishOp :: CInt,
   allocOp :: CInt,
   {-writeOp :: CInt,-}
   bindOp :: CInt,
   {-readOp :: CInt,-}
   -- Update
   {-fileOp :: CInt,-}
   {-finishOp :: CInt,-}
   bufferOp :: CInt,
   offsetOp :: CInt,
   widthOp :: CInt,
   sizeOp :: CInt,
   heightOp :: CInt,
   dataOp :: CInt,
   textOp :: CInt,
   scalarOp :: CInt,
   queryOp :: CInt,
   formatOp :: CInt,
   {-feedbackOp :: CInt,-}
   datasOp :: CInt,
   functionOp :: CInt,
   functionsOp :: CInt,
   -- Render
   {-fileOp :: CInt,-}
   programOp :: CInt,
   baseOp :: CInt,
   countOp :: CInt,
   {-sizeOp :: CInt,-}
   -- Feedback
   pierceOp :: CInt,
   normalOp :: CInt,
   tagbitsOp :: CInt,
   planeOp :: CInt,
   -- Format
   cursorOp :: CInt,
   affineOp :: CInt,
   perplaneOp :: CInt,
   basisOp :: CInt,
   cutoffOp :: CInt,
   slopeOp :: CInt,
   aspectOp :: CInt,
   featherOp :: CInt,
   arrowOp :: CInt,
   enableOp :: CInt,
   -- Data
   {-fileOp :: CInt,
   planeOp :: CInt,-}
   confOp :: CInt,
   boundariesOp :: CInt,
   regionsOp :: CInt,
   planesOp :: CInt,
   sidesOp :: CInt,
   sideOp :: CInt,
   insidesOp :: CInt,
   outsidesOp :: CInt,
   insideOp :: CInt,
   outsideOp :: CInt,
   versorOp :: CInt,
   vectorOp :: CInt,
   filenameOp :: CInt,
   keyOp :: CInt,
   funcOp :: CInt,
   {-countOp :: CInt,-}
   specifyOp :: CInt,
   {-scriptOp :: CInt,-}
   {-fixedOp :: CInt,-}
   relativeOp :: CInt,
   absoluteOp :: CInt,
   {-pierceOp :: CInt,-}
   {-matrixOp :: CInt,-}
   pressOp :: CInt,
   -- Sound
   {-fileOp :: CInt,-}
   identOp :: CInt,
   doneOp :: CInt,
   valueOp :: CInt,
   eventOp :: CInt,
   {-valueOp :: CInt,-}
   delayOp :: CInt,
   schedOp :: CInt,
   leftOp :: CInt,
   rightOp :: CInt,
   -- Equ
   numerOp :: CInt,
   denomOp :: CInt,
   -- Sum
   {-countOp :: CInt,-}
   -- Term
   coefOp :: CInt,
   factorOp :: CInt,
   constOp :: CInt,
   varyOp :: CInt,
   squareOp :: CInt,
   compOp :: CInt,
   -- State
   {-fileOp :: CInt,
   planeOp :: CInt,-}
   changeOp :: CInt,
   {-sculptOp :: CInt,
   clickOp :: CInt,
   mouseOp :: CInt,
   rollerOp :: CInt,
   targetOp :: CInt,
   topologyOp :: CInt,
   fixedOp :: CInt,-}
   {-sideOp :: CInt,
   insidesOp :: CInt,
   outsidesOp :: CInt,
   insideOp :: CInt,
   outsideOp :: CInt,-}
   {-matrixOp :: CInt,-}
   {-versorOp :: CInt,
   vectorOp :: CInt,-}
   {-textOp :: CInt,-}
   -- Query
   {-fileOp :: CInt,-}
   smartOp :: CInt,
   {-valueOp :: CInt,-}
   givenOp :: CInt,
   lengthOp :: CInt,
   doublesOp :: CInt,
   floatsOp :: CInt,
   intsOp :: CInt,
   opcodes :: CInt,
   -- Source
   configureSource :: CInt,
   modeSource :: CInt,
   matrixSource :: CInt,
   globalSource :: CInt,
   polytopeSource :: CInt,
   sources :: CInt,
   -- Subconf
   startSub :: CInt,
   stopSub :: CInt,
   subconfs :: CInt,
   -- Sculpt
   clickUlpt :: CInt,
   mouseUlpt :: CInt,
   rollerUlpt :: CInt,
   targetUlpt :: CInt,
   topologyUlpt :: CInt,
   fixedUlpt :: CInt,
   sculpts :: CInt,
   -- ClickMode
   additiveMode :: CInt,
   subtractiveMode :: CInt,
   refineMode :: CInt,
   tweakMode :: CInt,
   performMode :: CInt,
   transformMode :: CInt,
   suspendMode :: CInt,
   pierceMode :: CInt,
   clickModes :: CInt,
   -- MouseMode
   rotateMode :: CInt,
   tangentMode :: CInt,
   translateMode :: CInt,
   mouseModes :: CInt,
   -- RollerMode
   cylinderMode :: CInt,
   clockMode :: CInt,
   normalMode :: CInt,
   parallelMode :: CInt,
   scaleMode :: CInt,
   rollerModes :: CInt,
   -- TargetMode
   sessionMode :: CInt,
   polytopeMode :: CInt,
   facetMode :: CInt,
   targetModes :: CInt,
   -- TopologyMode
   numericMode :: CInt,
   invariantMode :: CInt,
   symbolicMode :: CInt,
   topologyModes :: CInt,
   -- FixedMode
   relativeMode :: CInt,
   absoluteMode :: CInt,
   fixedModes :: CInt,
   -- Field
   allocField :: CInt,
   writeField :: CInt,
   bindField :: CInt,
   readField :: CInt,
   fields :: CInt,
   -- Buffer
   point0 :: CInt,
   versor :: CInt,
   point1 :: CInt,
   normal :: CInt,
   coordinate :: CInt,
   color :: CInt,
   weight :: CInt,
   tag :: CInt,
   point2 :: CInt,
   face1 :: CInt,
   triple0 :: CInt,
   triple1 :: CInt,
   feedback :: CInt,
   uniform :: CInt,
   inquery :: CInt,
   texture0 :: CInt,
   texture1 :: CInt,
   buffers :: CInt,
   -- Program
   draw :: CInt,
   pierce :: CInt,
   sect0 :: CInt,
   sect1 :: CInt,
   side1 :: CInt,
   side2 :: CInt,
   programs :: CInt,
   -- Configure
   inflateConf :: CInt,
   spaceConf :: CInt,
   regionConf :: CInt,
   planeConf :: CInt,
   pictureConf :: CInt,
   onceConf :: CInt,
   changeConf :: CInt,
   macroConf :: CInt,
   hotkeyConf :: CInt,
   relativeConf :: CInt,
   absoluteConf :: CInt,
   refineConf :: CInt,
   manipConf :: CInt,
   pressConf :: CInt,
   clickConf :: CInt,  
   additiveConf :: CInt,
   subtractiveConf :: CInt,
   configures :: CInt,
   -- Function
   attachedFunc :: CInt,
   functions :: CInt,
   -- Event
   startEvent :: CInt,
   stopEvent :: CInt,
   soundEvent :: CInt,
   scriptEvent :: CInt,
   updateEvent :: CInt,
   events :: CInt,
   -- Equate
   valueEqu :: CInt,
   delayEqu :: CInt,
   schedEqu :: CInt,
   leftEqu :: CInt,
   rightEqu :: CInt,
   equates :: CInt,
   -- Factor
   constFactor :: CInt,
   varyFactor :: CInt,
   squareFactor :: CInt,
   compFactor :: CInt,
   factors :: CInt,
   -- Change
   sculptChange :: CInt,
   globalChange :: CInt,
   matrixChange :: CInt,
   planeChange :: CInt,
   regionChange :: CInt,
   textChange :: CInt,
   changes :: CInt,
   -- Given
   doublesGiv :: CInt,
   floatsGiv :: CInt,
   intsGiv :: CInt,
   givens :: CInt}

main = do
   print (holes 5 [2,3,4])
   setDebug (fromIntegral 1)
   [rdfd,wrfd] <- fmap (map read) getArgs
   -- Sideband
   -- TODO for new Query pointer
   -- Message
   readOpV <- (newCString "ReadOp") >>= enumerate
   writeOpV <- (newCString "WriteOp") >>= enumerate
   scriptOpV <- (newCString "ScriptOp") >>= enumerate
   windowOpV <- (newCString "WindowOp") >>= enumerate
   commandOpV <- (newCString "CommandOp") >>= enumerate
   -- Command
   fileOpV <- (newCString "FileOp") >>= enumerate
   sourceOpV <- (newCString "SourceOp") >>= enumerate
   subconfOpV <- (newCString "SubconfOp") >>= enumerate
   settingOpV <- (newCString "SettingOp") >>= enumerate
   sculptOpV <- (newCString "SculptOp") >>= enumerate
   clickOpV <- (newCString "ClickOp") >>= enumerate
   mouseOpV <- (newCString "MouseOp") >>= enumerate
   rollerOpV <- (newCString "RollerOp") >>= enumerate
   targetOpV <- (newCString "TargetOp") >>= enumerate
   topologyOpV <- (newCString "TopologyOp") >>= enumerate
   fixedOpV <- (newCString "FixedOp") >>= enumerate
   matrixOpV <- (newCString "MatrixOp") >>= enumerate
   feedbackOpV <- (newCString "FeedbackOp") >>= enumerate
   finishOpV <- (newCString "FinishOp") >>= enumerate
   allocOpV <- (newCString "AllocOp") >>= enumerate
   {-writeOpV <- (newCString "WriteOp") >>= enumerate-}
   bindOpV <- (newCString "BindOp") >>= enumerate
   {-readOpV <- (newCString "ReadOp") >>= enumerate-}
   -- Update
   {-fileOpV <- (newCString "FileOp") >>= enumerate-}
   {-finishOpV <- (newCString "FinishOp") >>= enumerate-}
   bufferOpV <- (newCString "BufferOp") >>= enumerate
   offsetOpV <- (newCString "OffsetOp") >>= enumerate
   widthOpV <- (newCString "WidthOp") >>= enumerate
   sizeOpV <- (newCString "SizeOp") >>= enumerate
   heightOpV <- (newCString "HeightOp") >>= enumerate
   dataOpV <- (newCString "DataOp") >>= enumerate
   textOpV <- (newCString "TextOp") >>= enumerate
   scalarOpV <- (newCString "ScalarOp") >>= enumerate
   queryOpV <- (newCString "QueryOp") >>= enumerate
   formatOpV <- (newCString "FormatOp") >>= enumerate
   {-feedbackOpV <- (newCString "FeedbackOp") >>= enumerate-}
   datasOpV <- (newCString "DatasOp") >>= enumerate
   functionOpV <- (newCString "FunctionOp") >>= enumerate
   functionsOpV <- (newCString "FunctionsOp") >>= enumerate
   -- Render
   {-fileOpV <- (newCString "FileOp") >>= enumerate-}
   programOpV <- (newCString "ProgramOp") >>= enumerate
   baseOpV <- (newCString "BaseOp") >>= enumerate
   countOpV <- (newCString "CountOp") >>= enumerate
   {-sizeOpV <- (newCString "SizeOp") >>= enumerate-}
   -- Feedback
   pierceOpV <- (newCString "PierceOp") >>= enumerate
   normalOpV <- (newCString "NormalOp") >>= enumerate
   tagbitsOpV <- (newCString "TagbitsOp") >>= enumerate
   planeOpV <- (newCString "PlaneOp") >>= enumerate
   -- Format
   cursorOpV <- (newCString "CursorOp") >>= enumerate
   affineOpV <- (newCString "AffineOp") >>= enumerate
   perplaneOpV <- (newCString "PerplaneOp") >>= enumerate
   basisOpV <- (newCString "BasisOp") >>= enumerate
   cutoffOpV <- (newCString "CutoffOp") >>= enumerate
   slopeOpV <- (newCString "SlopeOp") >>= enumerate
   aspectOpV <- (newCString "AspectOp") >>= enumerate
   featherOpV <- (newCString "FeatherOp") >>= enumerate
   arrowOpV <- (newCString "ArrowOp") >>= enumerate
   enableOpV <- (newCString "EnableOp") >>= enumerate
   -- Data
   {-fileOpV <- (newCString "FileOp") >>= enumerate
   planeOpV <- (newCString "PlaneOp") >>= enumerate-}
   confOpV <- (newCString "ConfOp") >>= enumerate
   boundariesOpV <- (newCString "BoundariesOp") >>= enumerate
   regionsOpV <- (newCString "RegionsOp") >>= enumerate
   planesOpV <- (newCString "PlanesOp") >>= enumerate
   sidesOpV <- (newCString "SidesOp") >>= enumerate
   sideOpV <- (newCString "SideOp") >>= enumerate
   insidesOpV <- (newCString "InsidesOp") >>= enumerate
   outsidesOpV <- (newCString "OutsidesOp") >>= enumerate
   insideOpV <- (newCString "InsideOp") >>= enumerate
   outsideOpV <- (newCString "OutsideOp") >>= enumerate
   versorOpV <- (newCString "VersorOp") >>= enumerate
   vectorOpV <- (newCString "VectorOp") >>= enumerate
   filenameOpV <- (newCString "FilenameOp") >>= enumerate
   keyOpV <- (newCString "KeyOp") >>= enumerate
   funcOpV <- (newCString "FuncOp") >>= enumerate
   {-countOpV <- (newCString "CountOp") >>= enumerate-}
   specifyOpV <- (newCString "SpecifyOp") >>= enumerate
   {-scriptOpV <- (newCString "ScriptOp") >>= enumerate-}
   {-fixedOpV <- (newCString "FixedOp") >>= enumerate-}
   relativeOpV <- (newCString "RelativeOp") >>= enumerate
   absoluteOpV <- (newCString "AbsoluteOp") >>= enumerate
   {-pierceOpV <- (newCString "PierceOp") >>= enumerate-}
   {-matrixOpV <- (newCString "MatrixOp") >>= enumerate-}
   pressOpV <- (newCString "PressOp") >>= enumerate
   -- Sound
   {-fileOpV <- (newCString "FileOp") >>= enumerate-}
   identOpV <- (newCString "IdentOp") >>= enumerate
   doneOpV <- (newCString "DoneOp") >>= enumerate
   valueOpV <- (newCString "ValueOp") >>= enumerate
   eventOpV <- (newCString "EventOp") >>= enumerate
   {-valueOpV <- (newCString "ValueOp") >>= enumerate-}
   delayOpV <- (newCString "DelayOp") >>= enumerate
   schedOpV <- (newCString "SchedOp") >>= enumerate
   leftOpV <- (newCString "LeftOp") >>= enumerate
   rightOpV <- (newCString "RightOp") >>= enumerate
   -- Equ
   numerOpV <- (newCString "NumerOp") >>= enumerate
   denomOpV <- (newCString "DenomOp") >>= enumerate
   -- Sum
   {-countOpV <- (newCString "CountOp") >>= enumerate-}
   -- Term
   coefOpV <- (newCString "CoefOp") >>= enumerate
   factorOpV <- (newCString "FactorOp") >>= enumerate
   constOpV <- (newCString "ConstOp") >>= enumerate
   varyOpV <- (newCString "VaryOp") >>= enumerate
   squareOpV <- (newCString "SquareOp") >>= enumerate
   compOpV <- (newCString "CompOp") >>= enumerate
   -- State
   {-fileOpV <- (newCString "FileOp") >>= enumerate
   planeOpV <- (newCString "PlaneOp") >>= enumerate-}
   changeOpV <- (newCString "ChangeOp") >>= enumerate
   {-sculptOpV <- (newCString "SculptOp") >>= enumerate
   clickOpV <- (newCString "ClickOp") >>= enumerate
   mouseOpV <- (newCString "MouseOp") >>= enumerate
   rollerOpV <- (newCString "RollerOp") >>= enumerate
   targetOpV <- (newCString "TargetOp") >>= enumerate
   topologyOpV <- (newCString "TopologyOp") >>= enumerate
   fixedOpV <- (newCString "FixedOp") >>= enumerate-}
   {-sideOpV <- (newCString "SideOp") >>= enumerate
   insidesOpV <- (newCString "InsidesOp") >>= enumerate
   outsidesOpV <- (newCString "OutsidesOp") >>= enumerate
   insideOpV <- (newCString "InsideOp") >>= enumerate
   outsideOpV <- (newCString "OutsideOp") >>= enumerate-}
   {-matrixOpV <- (newCString "MatrixOp") >>= enumerate-}
   {-versorOpV <- (newCString "VersorOp") >>= enumerate
   vectorOpV <- (newCString "VectorOp") >>= enumerate-}
   {-textOpV <- (newCString "TextOp") >>= enumerate-}
   -- Query
   {-fileOpV <- (newCString "FileOp") >>= enumerate-}
   smartOpV <- (newCString "SmartOp") >>= enumerate
   {-valueOpV <- (newCString "ValueOp") >>= enumerate-}
   givenOpV <- (newCString "GivenOp") >>= enumerate
   lengthOpV <- (newCString "LengthOp") >>= enumerate
   doublesOpV <- (newCString "DoublesOp") >>= enumerate
   floatsOpV <- (newCString "FloatsOp") >>= enumerate
   intsOpV <- (newCString "IntsOp") >>= enumerate
   opcodesV <- (newCString "Opcodes") >>= enumerate
   -- Source
   configureSourceV <- (newCString "ConfigureSource") >>= enumerate
   modeSourceV <- (newCString "ModeSource") >>= enumerate
   matrixSourceV <- (newCString "MatrixSource") >>= enumerate
   globalSourceV <- (newCString "GlobalSource") >>= enumerate
   polytopeSourceV <- (newCString "PolytopeSource") >>= enumerate
   sourcesV <- (newCString "Sources") >>= enumerate
   -- Subconf
   startSubV <- (newCString "StartSub") >>= enumerate
   stopSubV <- (newCString "StopSub") >>= enumerate
   subconfsV <- (newCString "Subconfs") >>= enumerate
   -- Sculpt
   clickUlptV <- (newCString "ClickUlpt") >>= enumerate
   mouseUlptV <- (newCString "MouseUlpt") >>= enumerate
   rollerUlptV <- (newCString "RollerUlpt") >>= enumerate
   targetUlptV <- (newCString "TargetUlpt") >>= enumerate
   topologyUlptV <- (newCString "TopologyUlpt") >>= enumerate
   fixedUlptV <- (newCString "FixedUlpt") >>= enumerate
   sculptsV <- (newCString "Sculpts") >>= enumerate
   -- ClickMode
   additiveModeV <- (newCString "AdditiveMode") >>= enumerate
   subtractiveModeV <- (newCString "SubtractiveMode") >>= enumerate
   refineModeV <- (newCString "RefineMode") >>= enumerate
   tweakModeV <- (newCString "TweakMode") >>= enumerate
   performModeV <- (newCString "PerformMode") >>= enumerate
   transformModeV <- (newCString "TransformMode") >>= enumerate
   suspendModeV <- (newCString "SuspendMode") >>= enumerate
   pierceModeV <- (newCString "PierceMode") >>= enumerate
   clickModesV <- (newCString "ClickModes") >>= enumerate
   -- MouseMode
   rotateModeV <- (newCString "RotateMode") >>= enumerate
   tangentModeV <- (newCString "TangentMode") >>= enumerate
   translateModeV <- (newCString "TranslateMode") >>= enumerate
   mouseModesV <- (newCString "MouseModes") >>= enumerate
   -- RollerMode
   cylinderModeV <- (newCString "CylinderMode") >>= enumerate
   clockModeV <- (newCString "ClockMode") >>= enumerate
   normalModeV <- (newCString "NormalMode") >>= enumerate
   parallelModeV <- (newCString "ParallelMode") >>= enumerate
   scaleModeV <- (newCString "ScaleMode") >>= enumerate
   rollerModesV <- (newCString "RollerModes") >>= enumerate
   -- TargetMode
   sessionModeV <- (newCString "SessionMode") >>= enumerate
   polytopeModeV <- (newCString "PolytopeMode") >>= enumerate
   facetModeV <- (newCString "FacetMode") >>= enumerate
   targetModesV <- (newCString "TargetModes") >>= enumerate
   -- TopologyMode
   numericModeV <- (newCString "NumericMode") >>= enumerate
   invariantModeV <- (newCString "InvariantMode") >>= enumerate
   symbolicModeV <- (newCString "SymbolicMode") >>= enumerate
   topologyModesV <- (newCString "TopologyModes") >>= enumerate
   -- FixedMode
   relativeModeV <- (newCString "RelativeMode") >>= enumerate
   absoluteModeV <- (newCString "AbsoluteMode") >>= enumerate
   fixedModesV <- (newCString "FixedModes") >>= enumerate
   -- Field
   allocFieldV <- (newCString "AllocField") >>= enumerate
   writeFieldV <- (newCString "WriteField") >>= enumerate
   bindFieldV <- (newCString "BindField") >>= enumerate
   readFieldV <- (newCString "ReadField") >>= enumerate
   fieldsV <- (newCString "Fields") >>= enumerate
   -- Buffer
   point0V <- (newCString "Point0") >>= enumerate
   versorV <- (newCString "Versor") >>= enumerate
   point1V <- (newCString "Point1") >>= enumerate
   normalV <- (newCString "Normal") >>= enumerate
   coordinateV <- (newCString "Coordinate") >>= enumerate
   colorV <- (newCString "Color") >>= enumerate
   weightV <- (newCString "Weight") >>= enumerate
   tagV <- (newCString "Tag") >>= enumerate
   point2V <- (newCString "Point2") >>= enumerate
   face1V <- (newCString "Face1") >>= enumerate
   triple0V <- (newCString "Triple0") >>= enumerate
   triple1V <- (newCString "Triple1") >>= enumerate
   feedbackV <- (newCString "Feedback") >>= enumerate
   uniformV <- (newCString "Uniform") >>= enumerate
   inqueryV <- (newCString "Inquery") >>= enumerate
   texture0V <- (newCString "Texture0") >>= enumerate
   texture1V <- (newCString "Texture1") >>= enumerate
   buffersV <- (newCString "Buffers") >>= enumerate
   -- Program
   drawV <- (newCString "Draw") >>= enumerate
   pierceV <- (newCString "Pierce") >>= enumerate
   sect0V <- (newCString "Sect0") >>= enumerate
   sect1V <- (newCString "Sect1") >>= enumerate
   side1V <- (newCString "Side1") >>= enumerate
   side2V <- (newCString "Side2") >>= enumerate
   programsV <- (newCString "Programs") >>= enumerate
   -- Configure
   inflateConfV <- (newCString "InflateConf") >>= enumerate
   spaceConfV <- (newCString "SpaceConf") >>= enumerate
   regionConfV <- (newCString "RegionConf") >>= enumerate
   planeConfV <- (newCString "PlaneConf") >>= enumerate
   pictureConfV <- (newCString "PictureConf") >>= enumerate
   onceConfV <- (newCString "OnceConf") >>= enumerate
   changeConfV <- (newCString "ChangeConf") >>= enumerate
   macroConfV <- (newCString "MacroConf") >>= enumerate
   hotkeyConfV <- (newCString "HotkeyConf") >>= enumerate
   relativeConfV <- (newCString "RelativeConf") >>= enumerate
   absoluteConfV <- (newCString "AbsoluteConf") >>= enumerate
   refineConfV <- (newCString "RefineConf") >>= enumerate
   manipConfV <- (newCString "ManipConf") >>= enumerate
   pressConfV <- (newCString "PressConf") >>= enumerate
   clickConfV <- (newCString "ClickConf") >>= enumerate  
   additiveConfV <- (newCString "AdditiveConf") >>= enumerate
   subtractiveConfV <- (newCString "SubtractiveConf") >>= enumerate
   configuresV <- (newCString "Configures") >>= enumerate
   -- Function
   attachedFuncV <- (newCString "AttachedFunc") >>= enumerate
   functionsV <- (newCString "Functions") >>= enumerate
   -- Event
   startEventV <- (newCString "StartEvent") >>= enumerate
   stopEventV <- (newCString "StopEvent") >>= enumerate
   soundEventV <- (newCString "SoundEvent") >>= enumerate
   scriptEventV <- (newCString "ScriptEvent") >>= enumerate
   updateEventV <- (newCString "UpdateEvent") >>= enumerate
   eventsV <- (newCString "Events") >>= enumerate
   -- Equate
   valueEquV <- (newCString "ValueEqu") >>= enumerate
   delayEquV <- (newCString "DelayEqu") >>= enumerate
   schedEquV <- (newCString "SchedEqu") >>= enumerate
   leftEquV <- (newCString "LeftEqu") >>= enumerate
   rightEquV <- (newCString "RightEqu") >>= enumerate
   equatesV <- (newCString "Equates") >>= enumerate
   -- Factor
   constFactorV <- (newCString "ConstFactor") >>= enumerate
   varyFactorV <- (newCString "VaryFactor") >>= enumerate
   squareFactorV <- (newCString "SquareFactor") >>= enumerate
   compFactorV <- (newCString "CompFactor") >>= enumerate
   factorsV <- (newCString "Factors") >>= enumerate
   -- Change
   sculptChangeV <- (newCString "SculptChange") >>= enumerate
   globalChangeV <- (newCString "GlobalChange") >>= enumerate
   matrixChangeV <- (newCString "MatrixChange") >>= enumerate
   planeChangeV <- (newCString "PlaneChange") >>= enumerate
   regionChangeV <- (newCString "RegionChange") >>= enumerate
   textChangeV <- (newCString "TextChange") >>= enumerate
   changesV <- (newCString "Changes") >>= enumerate
   -- Given
   doublesGivV <- (newCString "DoublesGiv") >>= enumerate
   floatsGivV <- (newCString "FloatsGiv") >>= enumerate
   intsGivV <- (newCString "IntsGiv") >>= enumerate
   givensV <- (newCString "Givens") >>= enumerate
   mainLoop rdfd wrfd Enumeration {
   -- Sideband
   -- TODO for new Query pointer
   -- Message
   readOp = readOpV,
   writeOp = writeOpV,
   scriptOp = scriptOpV,
   windowOp = windowOpV,
   commandOp = commandOpV,
   -- Command
   fileOp = fileOpV,
   sourceOp = sourceOpV,
   subconfOp = subconfOpV,
   settingOp = settingOpV,
   sculptOp = sculptOpV,
   clickOp = clickOpV,
   mouseOp = mouseOpV,
   rollerOp = rollerOpV,
   targetOp = targetOpV,
   topologyOp = topologyOpV,
   fixedOp = fixedOpV,
   matrixOp = matrixOpV,
   feedbackOp = feedbackOpV,
   finishOp = finishOpV,
   allocOp = allocOpV,
   {-writeOp = writeOpV,-}
   bindOp = bindOpV,
   {-readOp = readOpV,-}
   -- Update
   {-fileOp = fileOpV,-}
   {-finishOp = finishOpV,-}
   bufferOp = bufferOpV,
   offsetOp = offsetOpV,
   widthOp = widthOpV,
   sizeOp = sizeOpV,
   heightOp = heightOpV,
   dataOp = dataOpV,
   textOp = textOpV,
   scalarOp = scalarOpV,
   queryOp = queryOpV,
   formatOp = formatOpV,
   {-feedbackOp = feedbackOpV,-}
   datasOp = datasOpV,
   functionOp = functionOpV,
   functionsOp = functionsOpV,
   -- Render
   {-fileOp = fileOpV,-}
   programOp = programOpV,
   baseOp = baseOpV,
   countOp = countOpV,
   {-sizeOp = sizeOpV,-}
   -- Feedback
   pierceOp = pierceOpV,
   normalOp = normalOpV,
   tagbitsOp = tagbitsOpV,
   planeOp = planeOpV,
   -- Format
   cursorOp = cursorOpV,
   affineOp = affineOpV,
   perplaneOp = perplaneOpV,
   basisOp = basisOpV,
   cutoffOp = cutoffOpV,
   slopeOp = slopeOpV,
   aspectOp = aspectOpV,
   featherOp = featherOpV,
   arrowOp = arrowOpV,
   enableOp = enableOpV,
   -- Data
   {-fileOp = fileOpV,
   planeOp = planeOpV,-}
   confOp = confOpV,
   boundariesOp = boundariesOpV,
   regionsOp = regionsOpV,
   planesOp = planesOpV,
   sidesOp = sidesOpV,
   sideOp = sideOpV,
   insidesOp = insidesOpV,
   outsidesOp = outsidesOpV,
   insideOp = insideOpV,
   outsideOp = outsideOpV,
   versorOp = versorOpV,
   vectorOp = vectorOpV,
   filenameOp = filenameOpV,
   keyOp = keyOpV,
   funcOp = funcOpV,
   {-countOp = countOpV,-}
   specifyOp = specifyOpV,
   {-scriptOp = scriptOpV,-}
   {-fixedOp = fixedOpV,-}
   relativeOp = relativeOpV,
   absoluteOp = absoluteOpV,
   {-pierceOp = pierceOpV,-}
   {-matrixOp = matrixOpV,-}
   pressOp = pressOpV,
   -- Sound
   {-fileOp = fileOpV,-}
   identOp = identOpV,
   doneOp = doneOpV,
   valueOp = valueOpV,
   eventOp = eventOpV,
   {-valueOp = valueOpV,-}
   delayOp = delayOpV,
   schedOp = schedOpV,
   leftOp = leftOpV,
   rightOp = rightOpV,
   -- Equ
   numerOp = numerOpV,
   denomOp = denomOpV,
   -- Sum
   {-countOp = countOpV,-}
   -- Term
   coefOp = coefOpV,
   factorOp = factorOpV,
   constOp = constOpV,
   varyOp = varyOpV,
   squareOp = squareOpV,
   compOp = compOpV,
   -- State
   {-fileOp = fileOpV,
   planeOp = planeOpV,-}
   changeOp = changeOpV,
   {-sculptOp = sculptOpV,
   clickOp = clickOpV,
   mouseOp = mouseOpV,
   rollerOp = rollerOpV,
   targetOp = targetOpV,
   topologyOp = topologyOpV,
   fixedOp = fixedOpV,-}
   {-sideOp = sideOpV,
   insidesOp = insidesOpV,
   outsidesOp = outsidesOpV,
   insideOp = insideOpV,
   outsideOp = outsideOpV,-}
   {-matrixOp = matrixOpV,-}
   {-versorOp = versorOpV,
   vectorOp = vectorOpV,-}
   {-textOp = textOpV,-}
   -- Query
   {-fileOp = fileOpV,-}
   smartOp = smartOpV,
   {-valueOp = valueOpV,-}
   givenOp = givenOpV,
   lengthOp = lengthOpV,
   doublesOp = doublesOpV,
   floatsOp = floatsOpV,
   intsOp = intsOpV,
   opcodes = opcodesV,
   -- Source
   configureSource = configureSourceV,
   modeSource = modeSourceV,
   matrixSource = matrixSourceV,
   globalSource = globalSourceV,
   polytopeSource = polytopeSourceV,
   sources = sourcesV,
   -- Subconf
   startSub = startSubV,
   stopSub = stopSubV,
   subconfs = subconfsV,
   -- Sculpt
   clickUlpt = clickUlptV,
   mouseUlpt = mouseUlptV,
   rollerUlpt = rollerUlptV,
   targetUlpt = targetUlptV,
   topologyUlpt = topologyUlptV,
   fixedUlpt = fixedUlptV,
   sculpts = sculptsV,
   -- ClickMode
   additiveMode = additiveModeV,
   subtractiveMode = subtractiveModeV,
   refineMode = refineModeV,
   tweakMode = tweakModeV,
   performMode = performModeV,
   transformMode = transformModeV,
   suspendMode = suspendModeV,
   pierceMode = pierceModeV,
   clickModes = clickModesV,
   -- MouseMode
   rotateMode = rotateModeV,
   tangentMode = tangentModeV,
   translateMode = translateModeV,
   mouseModes = mouseModesV,
   -- RollerMode
   cylinderMode = cylinderModeV,
   clockMode = clockModeV,
   normalMode = normalModeV,
   parallelMode = parallelModeV,
   scaleMode = scaleModeV,
   rollerModes = rollerModesV,
   -- TargetMode
   sessionMode = sessionModeV,
   polytopeMode = polytopeModeV,
   facetMode = facetModeV,
   targetModes = targetModesV,
   -- TopologyMode
   numericMode = numericModeV,
   invariantMode = invariantModeV,
   symbolicMode = symbolicModeV,
   topologyModes = topologyModesV,
   -- FixedMode
   relativeMode = relativeModeV,
   absoluteMode = absoluteModeV,
   fixedModes = fixedModesV,
   -- Field
   allocField = allocFieldV,
   writeField = writeFieldV,
   bindField = bindFieldV,
   readField = readFieldV,
   fields = fieldsV,
   -- Buffer
   point0 = point0V,
   versor = versorV,
   point1 = point1V,
   normal = normalV,
   coordinate = coordinateV,
   color = colorV,
   weight = weightV,
   tag = tagV,
   point2 = point2V,
   face1 = face1V,
   triple0 = triple0V,
   triple1 = triple1V,
   feedback = feedbackV,
   uniform = uniformV,
   inquery = inqueryV,
   texture0 = texture0V,
   texture1 = texture1V,
   buffers = buffersV,
   -- Program
   draw = drawV,
   pierce = pierceV,
   sect0 = sect0V,
   sect1 = sect1V,
   side1 = side1V,
   side2 = side2V,
   programs = programsV,
   -- Configure
   inflateConf = inflateConfV,
   spaceConf = spaceConfV,
   regionConf = regionConfV,
   planeConf = planeConfV,
   pictureConf = pictureConfV,
   onceConf = onceConfV,
   changeConf = changeConfV,
   macroConf = macroConfV,
   hotkeyConf = hotkeyConfV,
   relativeConf = relativeConfV,
   absoluteConf = absoluteConfV,
   refineConf = refineConfV,
   manipConf = manipConfV,
   pressConf = pressConfV,
   clickConf = clickConfV,  
   additiveConf = additiveConfV,
   subtractiveConf = subtractiveConfV,
   configures = configuresV,
   -- Function
   attachedFunc = attachedFuncV,
   functions = functionsV,
   -- Event
   startEvent = startEventV,
   stopEvent = stopEventV,
   soundEvent = soundEventV,
   scriptEvent = scriptEventV,
   updateEvent = updateEventV,
   events = eventsV,
   -- Equate
   valueEqu = valueEquV,
   delayEqu = delayEquV,
   schedEqu = schedEquV,
   leftEqu = leftEquV,
   rightEqu = rightEquV,
   equates = equatesV,
   -- Factor
   constFactor = constFactorV,
   varyFactor = varyFactorV,
   squareFactor = squareFactorV,
   compFactor = compFactorV,
   factors = factorsV,
   -- Change
   sculptChange = sculptChangeV,
   globalChange = globalChangeV,
   matrixChange = matrixChangeV,
   planeChange = planeChangeV,
   regionChange = regionChangeV,
   textChange = textChangeV,
   changes = changesV,
   -- Given
   doublesGiv = doublesGivV,
   floatsGiv = floatsGivV,
   intsGiv = intsGivV,
   givens = givensV} emptyState
