CMesh
고정된 모델 좌표계를 가지고 있기 때문에 OBB가 아닌 고정된 AABB를 사용

처음에는 Object에만 bounding box를 넣으려 했는데, mesh에 맞춰서 bounding box를 만들기에는 어려움을 느낌.
따라서 모든 mesh에 bounding box를 만들어서 그것을 merge해서 object의 bounding box로 만들기로 결정
merge 함수가 있는 AABB를 사용하기로 했고, mesh는 고정된 모델 좌표를 가지고 있기 때문에 OOB를 사용하지 않아도 되겠다고 판단
BoundingOrientedBox::CreateFromBoundingBox 함수

실행 동안 mesh는 바뀌지 않는다고 가정