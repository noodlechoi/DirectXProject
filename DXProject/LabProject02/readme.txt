class GraphicsPipeline => 싱글턴 패턴으로 하나만 생성
static CPoint3D WorldTransform(CPoint3D&, CObject);
static CPoint3D CameraTransform(CPoint3D&, CObject);
=> 오브젝트, 카메라 속성이 필요함. 

class Polygon
vertex를 vector로
CMesh에서 vector의 원소로 사용하기 때문에 이동 의미론 구현

